import datetime
import re

import numpy as np
import pandas as pd


def scan2xy(scan, x0=np.array([0, 0])):
    """ Convert a scan in angle-distance format to x,y  where 0,0 is the location of the scanner

    Parameters
    ----------
    scan: ndarray
        scan in angle-distance format [angle, dist]
    x0: ndarray, optional
        scanner coordinates, default [0,0]

    Returns
    -------
    xy_scan: ndarray
        scan converted  to xy coordinates
    """
    x = scan[:, 1] * np.cos(scan[:, 0])
    y = scan[:, 1] * np.sin(scan[:, 0])

    return np.c_[x, y] + x0


def rotate_scan(scan, angle):
    """Rotate a set of points in [x,y] format

    Parameters
    ----------
    scan: ndarray
        set of points (x,y) constituting the scan
    angle: float
        rotation angle in radians

    Returns
    -------
    scan_r: ndarray
        rotated points
    """
    scan_r = np.c_[scan[:, 0] * np.cos(angle) - scan[:, 1] * np.sin(angle),
                   scan[:, 0] * np.sin(angle) + scan[:, 1] * np.cos(angle)]

    return scan_r


def load_motion_file(filepath):
    """

    Parameters
    ----------
    filepath

    Returns
    -------

    """

    # start_time = datetime.datetime.strptime('2021-01-27 17:54:31.00', '%Y-%m-%d %H:%M:%S.%f').timestamp() + 1

    # get motion history
    mot = pd.read_csv(filepath, names=['ts', 'dir', 'speed']).iloc[1:].reset_index(
        drop=True)

    # mot['ts'] = mot['ts']  # normalize the timestamp to the start_time
    mot = pd.concat([pd.DataFrame({'ts': mot['ts'].min()-500, 'dir': ['stop']}), mot],
                    ignore_index=True)  # add stop at the beggining

    mot['ts_start'] = mot['ts']
    mot['ts_end'] = np.r_[mot['ts'].values[1:], mot['ts'].max() + 20]

    return mot


def load_scans(filepath, d_limit=(0.2, 15), min_size=50):
    # read scan file
    with open(filepath, 'r') as f:
        file = f.read()

    # search for individual scans {ts, angles [], dists []} using regex
    search = re.findall(r'(\d*\.\d*)x(\[(\d*[ x])*\d*\])x(\[(\d*[ x])*\d*\])', file.replace('\n', 'x'))

    # extract scans
    scan_ts = []
    scans = []

    for scan in search[1:]:

        # get angles
        astr = scan[1].replace('x', ' ').replace('   ', ' ')[1:-1]
        a = np.radians(np.fromstring(astr, sep=' ').astype('float') / 1000)

        # get distances
        dstr = scan[3].replace('x', ' ').replace('   ', ' ')[1:-1]
        d = np.fromstring(dstr, sep=' ').astype('float') / 100

        # construct scan table and remove outlier values (very small and very large)
        scan_ad = np.c_[a, d]
        scan_ad = scan_ad[scan_ad[:, 1] > d_limit[0]]
        scan_ad = scan_ad[scan_ad[:, 1] < d_limit[1]]

        if scan_ad.shape[0] > min_size:
            # get ts
            scan_ts.append(float(scan[0]))
            scans.append(scan_ad)

    return scans, np.array(scan_ts)


def extract_lines(s, angles, angle_dist=np.pi / 6, split_th=0.05, min_len=0.2, min_points=5):
    lines = []
    line_points = []

    for i1 in np.arange(0, 2 * np.pi, angle_dist):
        i2 = (i1 + angle_dist)
        ix = np.argwhere((i1 < angles) & (angles < i2))
        p = s[ix.T[0]]
        polys, points = split_into_lines(p, split_th, min_len, min_points)
        lines += polys
        line_points += points

    return np.array(lines), line_points


def merge_lines(segments, line_points, max_dist, a_tol, b_tol):
    end_pts = []
    for i in range(len(line_points)):
        end_pts.append(np.c_[line_points[i][0], line_points[i][-1]].T)

    end_pts = np.array(end_pts)

    lines = segments.copy()
    # lines[:, 0] = np.abs(lines[:, 0])
    # print(lines)
    i = 0
    to_merge = []
    same_line = []
    merging = False
    angles = np.arctan2(lines[:, 0], np.ones(lines[:, 0].shape))
    # print(angles)
    while i < len(lines):

        # print('dist: ', np.linalg.norm(end_pts[i,1] - end_pts[i+1,0]), 'coeff dist: ', np.abs(lines[i]-lines[i+1]))

        # check if the points are mergable
        # print(np.linalg.norm(end_pts[i, 1] - end_pts[(i + 1) % 1, 0]), np.abs(
        #         angles[i] - angles[(i + 1) % len(lines)]), lines[i, 1] - lines[(i + 1) % len(lines), 1])
        if np.linalg.norm(end_pts[i, 1] - end_pts[(i + 1) % len(lines), 0]) < max_dist and np.abs(
                angles[i] - angles[(i + 1) % len(lines)]) < a_tol and np.abs(lines[i, 1] - lines[(i + 1) % len(lines), 1]) < b_tol:

            if merging:  # if already merging append next index to list
                same_line.append((i + 1) % len(lines))
            else:  # if not merging start a new list
                same_line = [i, (i + 1) % len(lines)]
                merging = True

            i = i + 1  # check the next segment

        else:  # segment not mergable
            if merging:  # stop merging
                merging = False
                to_merge.append(same_line)  # append meergable indices to final to merge list
                # do not increment - check for the same index if its mergable with the next segment
            else:
                i = i + 1
    # print(to_merge)
    # merge lines:
    lines_am = []
    points_am = []
    for m in to_merge:
        pm = np.vstack([line_points[i] for i in m])
        points_am.append(pm)
        lines_am.append(np.polyfit(pm[:, 0], pm[:, 1], 1))

    # get indices of segments, which were merged
    merge_ind = []
    for mi in to_merge:
        merge_ind += mi

        # append not merged lines
    for i in range(len(segments)):
        if i not in merge_ind:
            lines_am.append(segments[i])
            points_am.append(line_points[i])

    return lines_am, points_am


def split_into_lines(p, split_th, min_len, min_points):
    if p.shape[0] < min_points:
        return [], []
    # get line params between two edge points
    pol = pair2line(p[0], p[-1])

    # calculate distance
    d = dist_from_line(p, pol)

    if d.max() > split_th:

        ix_max = np.argmax(d)  # split point

        # invoke for split set of points
        pol1, p1 = split_into_lines(p[:ix_max], split_th, min_len, min_points)
        pol2, p2 = split_into_lines(p[ix_max:], split_th, min_len, min_points)

        # concatenate results
        pol = pol1 + pol2
        pout = p1 + p2

        return pol, pout

    else:  # if no distinct split point - segment is whole
        if np.linalg.norm(p[0] - p[-1]) > min_len and p.shape[0] > min_points:
            return [pol], [p]
        else:
            return [], []


def dist_from_line(xy, sl_in):
    """ Calculate distance from a line

    Parameters
    ----------
    xy: ndarray
        point coordinates
    sl_in: ndarray
        slope and intercept of the line

    Returns
    -------
    d: ndarray
        distances of points from xy from the line
    """

    return np.abs(xy[:, 0] * sl_in[0] - xy[:, 1] + sl_in[1]) / np.sqrt(1 + sl_in[0] ** 2)


def pair2line(p1, p2):
    """

    Parameters
    ----------
    p1
    p2

    Returns
    -------

    """
    a = (p2[1] - p1[1]) / (p2[0] - p1[0])
    b = p1[1] - a * p1[0]

    return np.array((a, b))


def closest_point(xy, p):
    x = (xy[0] + p[0] * xy[0] - p[0] * p[1]) / (p[0] ** 2 + 1)
    y = (p[0] * (xy[0] + p[0] * xy[0]) + p[1]) / (p[0] ** 2 + 1)
    return np.r_[x, y]


def closest_point_on_line(p, xy):
    x = (xy[0] + p[0] * xy[1] - p[0] * p[1]) / (p[0] ** 2 + 1)
    y = (p[0] * (xy[0] + p[0] * xy[1]) + p[1]) / (p[0] ** 2 + 1)
    return np.r_[x, y]


def get_constraint(t1, t0, motion, LIN_SPEED, ROT_SPEED_LEFT, ROT_SPEED_RIGHT, h0=0, control_format=False):
    mot = motion[(motion.ts > t0) & (motion.ts < t1)].reset_index()

    h = h0
    x = np.r_[0, 0, 0].astype('float')
    i = 0
    for i, r in mot.iterrows():
        ts_start = np.maximum(r['ts_start'], t0)
        ts_end = np.minimum(r['ts_end'], t1)

        dt = ts_end - ts_start

        direction = r.dir

        if control_format:
            uv = np.r_[1.0, 0.0]
        else:
            uv = np.r_[np.cos(h), np.sin(h)]

        if direction == 'forward':
            x[:2] += uv * dt * LIN_SPEED

        if direction == 'reverse':
            x[:2] += -dt * uv * LIN_SPEED

        if direction == 'left':
            x[2] += dt * ROT_SPEED_LEFT
            h += dt * ROT_SPEED_LEFT

        if direction == 'right':
            x[2] += - dt * ROT_SPEED_RIGHT
            h += - dt * ROT_SPEED_RIGHT

    return x


def get_controls(t1, t0, motion, LIN_SPEED, ROT_SPEED_LEFT, ROT_SPEED_RIGHT, h0=0, control_format=False):
    mot = motion[(motion.ts_end >= t0) & (motion.ts_start <= t1)].reset_index()

    h = h0
    u = np.r_[0, 0].astype('float')
    i = 0
    for i, r in mot.iterrows():
        ts_start = np.maximum(r['ts_start'], t0)
        ts_end = np.minimum(r['ts_end'], t1)

        dt = ts_end - ts_start

        direction = r.dir

        if direction == 'forward':
            u[0] += dt * LIN_SPEED

        if direction == 'reverse':
            u[0] += -dt * LIN_SPEED

        if direction == 'left':
            u[1] += dt * ROT_SPEED_LEFT
            h += dt * ROT_SPEED_LEFT

        if direction == 'right':
            u[1] -= dt * ROT_SPEED_RIGHT
            h -= dt * ROT_SPEED_RIGHT

    return u


def rotate_line(p, theta, t):
    x = np.array([0, 1])
    y = np.polyval(p, x)

    t = np.array([[np.cos(theta), -np.sin(theta)], [np.sin(theta), np.cos(theta)]]).dot(t)

    xy = rotate_scan(np.c_[x, y], theta) + t

    pr = pair2line(xy[0], xy[1])

    return pr

import numpy as np

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

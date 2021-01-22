"""
The scanse module contains functions for interaction with the scanse scanner.
"""
import threading
import numpy as np
import datetime
import time
from sweeppy import Sweep


class Scanner(threading.Thread):
    """
    A Scanner thread gets scans fom the sensor and puts them into a FIFO queue
    """

    def __init__(self, dev, queue, stop_criterion):
        super().__init__()
        self.dev = dev
        self.queue = queue
        self.counter = 0
        self.stop_criterion = stop_criterion

    def run(self):
        """
        Start scanner thread. Puts the scans into a queue until the `stop_criterion` is achieved
        The scan is put in a dict alongside a timestamp
        """
        with Sweep(self.dev) as sweep:
            sweep.start_scanning()

            try:
                for scan in sweep.get_scans():
                    if self.stop_criterion.is_set():
                        sweep.stop_scanning()
                        print(sweep.get_sample_rate())
                        self.queue.put_nowait(None)

                        sweep.set_motor_speed(0) # turn off scanner
                        print("gathered {} scans".format(self.counter))
                        break
                    else:
                        self.queue.put_nowait({'time': datetime.datetime.now().timestamp(), 'scan': scan})
                        self.counter += 1

            except RuntimeError as e:
                print(e)
                self.queue.put_nowait(None)
                sweep.stop_scanning()
                sweep.set_motor_speed(0)  # turn off scanner
                print("gathered {} scans".format(self.counter))

class ScanGetter(threading.Thread):
    """
    ScanGetter extracts scans from the referenced queue.
    """

    def __init__(self, queue, scan_file):
        super().__init__()
        self.queue = queue
        self.scans = []
        self.scan_file = scan_file

    def run(self):
        """
        Iterate over the queue's scans blocking if the queue is empty. If the value in queue is None leave thread.
        """
        while True:
            scan = self.queue.get()

            if not scan:
                break

            self.scans.append(scan)
            a, d, s = parse_scan(scan['scan'])
            self.scan_file.write(str(scan['time']) + '\n')
            self.scan_file.write(np.array_str(a) + '\n')
            self.scan_file.write(np.array_str(d) + '\n')
            self.scan_file.flush()

            self.queue.task_done()


def parse_scan(scan):
    angle = []
    distance = []
    strength = []

    for sample in scan.samples:
        angle.append(sample.angle)
        distance.append(sample.distance)
        strength.append(sample.signal_strength)
    print(len(angle))
    return np.array(angle), np.array(distance), np.array(strength)
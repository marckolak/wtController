"""
The robot module contains classes and functions implementing the platform controller.
"""
import datetime
import time
import queue
import threading
import numpy as np
from sweeppy import Sweep

from wild_thumper.scanse import Scanner, ScanGetter

class Robot:
    """
    Class representing the robotic platform. Contains references to objects controlling motors and sensors.
    """

    speed_dict = {"forward": np.array([1, 1]),
                  "reverse": np.array([-1, -1]),
                  "left": np.array([-1, 1]),
                  "right": np.array([1, -1]),
                  "stop": np.array([0, 0])}

    def __init__(self, motor_left=None, motor_right=None, scanner=None):
        """Constructor

        Parameters
        ----------
        motor_left:
            left motor controller
        motor_right:
            right motor controller
        scanner:
            interface to control the scanner
        """

        self.motor_left = motor_left  # left motor controller
        self.motor_right = motor_right  # right motor controller
        self.scanner_port = scanner  # LiDAR

        self.motion_history = []  # motion history ts - motion parameters (for dead reckoning)

        self.start_timestamp = datetime.datetime.now().timestamp()
        session_start = datetime.datetime.now().strftime("%Y_%m_%d_%H_%M_%S")

        self.motion_file = open('slam_{}_motion.txt'.format(session_start), 'a+')
        self.scan_file = open('slam_{}_scan.txt'.format(session_start), 'a+')

        self.scanning_done = None

    def move(self, direction, speed):
        """Move the robot. Sets speeds. To stop the robot invoke again with `direction` equal to `'stop'`.

        Parameters
        ----------
        direction: str
            movement direction 'forward', 'reverse, 'stop', 'left', 'right', 'arc'
        speed: float
            speed in -1 to 1 range

        """
        if direction in Robot.speed_dict.keys():
            speed_l, speed_r = Robot.speed_dict[direction] * speed  # get L/R motor speeds
            print('speed set to {}, {}'.format(speed_l, speed_r))
            self.motor_left.set_target_speed(speed_l)
            self.motor_right.set_target_speed(speed_r)

        elif direction == 'arc':
            pass

        else:  # stop just in case
            self.motor_left.set_target_speed(0)
            self.motor_right.set_target_speed(0)

        # add to history for movement tracking reasons
        set_time = datetime.datetime.now().timestamp() - self.start_timestamp
        self.motion_history.append([set_time, direction, speed])
        self.motion_file.write('{},{},{}\n'.format(set_time, direction, speed))
        self.motion_file.flush()

    def process_scan_message(self, payload):

        try:
            if payload['action'] == 'start':
                self.start_scanner(speed=payload['speed'], rate=payload['rate'])

            elif payload['action'] == 'stop':
                self.scanning_done.set()
        except Exception:
            pass

    def stop_scanner(self):

        try:
            with Sweep(self.scanner_port) as sweep:
                sweep.set_motor_speed(0)
                print("Scanner stopped")
        except RuntimeError as e:
            print('Scanner error: ' + str(e))

    def start_scanner(self, speed, rate):

        try:
            print(type(speed))
            with Sweep(self.scanner_port) as sweep:
                sweep.set_motor_speed(2)
                # sweep.set_sample_rate(790)
                for i in range(20):
                    ready = sweep.get_motor_ready()
                    if ready:
                        print('scanner ready to scan')
                        print(sweep.get_sample_rate())
                        break
                    else:
                        time.sleep(1)

            self.scanning_done = threading.Event()
            fifo = queue.Queue()
            scanner = Scanner(self.scanner_port, fifo, self.scanning_done)
            getter = ScanGetter(fifo, self.scan_file)

            scanner.start()
            getter.start()

        except RuntimeError as e:
            print('Scanner error: ' + str(e))

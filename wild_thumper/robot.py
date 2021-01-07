"""
The robot module contains classes and functions implementing the platform controller.
"""
import datetime

import numpy as np


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
        self.scanner = scanner  # LiDAR

        self.motion_history = []  # motion history ts - motion parameters (for dead reckoning)

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
        self.motion_history.append([datetime.datetime.now(), direction, speed])

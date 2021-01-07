"""
The movement module contains functions allowing for movement planning and execution.
"""
import threading
import time


class MovementThread(threading.Thread):
    """
    Movement thread is used to execute
    """

    def __init__(self, robot, motion_sequence):
        """ Initialize thread

        Parameters
        ----------
        robot: wild_thumper.robot.Robot
            robot controller object
        motion_sequence
        """
        super().__init__()
        self.robot = robot
        self.motion_sequence = motion_sequence

    def run(self):
        """
        Run the thread. Execute path given in the movement sequence. Stop at the end
        """
        try:
            for m in self.motion_sequence:
                self.robot.move(m['direction'], m['speed'])

                time.sleep(m['time'])
        finally:
            self.robot.move('stop', 0)

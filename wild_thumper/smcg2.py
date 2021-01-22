"""
The smcg2 module contains functions used for controlling motors using Simple Motoro Controller G2 from Pololu.
"""
from serial import PortNotOpenError, Serial


class SmcG2Serial(object):
    """
    An object allowing the platform controller to communicate with SMC G2 over USB.
    Taken from Pololu documentation
    """

    def __init__(self, port, device_number=None, dummy=False):
        """
        Constructor

        Parameters
        ----------
        port: serial.Serial
            USB port object
        device_number:
            needed when there are two devices on a line and Pololu protocol is used
        """
        self.port = port
        self.device_number = device_number
        self.dummy = dummy

    def send_command(self, cmd, *data_bytes):
        """Sends command to the controller

        Parameters
        ----------
        cmd: byte
            command
        data_bytes: bytes
            data bytes e.g. speed value
        """
        try:
            if self.device_number is None:
                header = [cmd]  # Compact protocol
            else:
                header = [0xAA, self.device_number, cmd & 0x7F]  # Pololu protocol
            self.port.write(bytes(header + list(data_bytes)))
        except PortNotOpenError as e:
            if self.dummy:
                pass
            else:
                raise e

    def exit_safe_start(self):
        """
        Sends the Exit Safe Start command, which is required to drive the motor.
        """
        self.send_command(0x83)

    def set_target_speed(self, speed):
        """ Sets the SMC's target speed (-1 to 1) scaled to (-3200 to 3200).

        Parameters
        ----------
        speed: float
            speed in [-1,1] range, negative speed for backward movement

        """
        speed = int(speed * 3200)
        cmd = 0x85  # Motor forward
        if speed < 0:
            cmd = 0x86  # Motor reverse
            speed = -speed
        self.send_command(cmd, speed & 0x1F, speed >> 5 & 0x7F)

    def get_variable(self, id):
        """ Gets the specified variable as an unsigned value.

        Parameters
        ----------
        id: int
            variable id [0- error status]

        Returns
        -------
        value: int
        """
        self.send_command(0xA1, id)
        result = self.port.read(2)
        if len(result) != 2:
            raise RuntimeError("Expected to read 2 bytes, got {}."
                               .format(len(result)))
        b = bytearray(result)
        return b[0] + 256 * b[1]

    def get_variable_signed(self, id):
        """Gets the specified variable as a signed value.

        Parameters
        ----------
        id: int
            variable id [0- error status]

        Returns
        -------
        value: int
        """
        value = self.get_variable(id)
        if value >= 0x8000:
            value -= 0x10000
        return value

    def get_target_speed(self):
        """ Gets the target speed (-3200 to 3200) scaled to (-1, 1).

        Returns
        -------
        speed: float
            target speed scaled to (-1, 1)
        """
        return float(self.get_variable_signed(20)) / 3200

    def get_error_status(self):
        """
        Gets a number where each bit represents a different error, and the
        bit is 1 if the error is currently active.
        See the user's guide for definitions of the different error bits.

        Returns
        -------
        error_status: int
            error status
        """
        return self.get_variable(0)


def open_port(port_name, baud_rate=9600, timeout=0.1, write_timeout=0.1):
    """Open USB port using pyserial package

    Parameters
    ----------
    port_name: str
        port name e.g. `/dev/USB0` or `COM5`
    baud_rate: int
        not needed if using USB
    timeout: float
    write_timeout: float

    Returns
    -------
    port: pyserial.Port
        opened USB port
    """
    port = Serial(port_name, timeout=0.1, write_timeout=0.1)

    return port

"""
The cir module contains functions for interaction with a CIR collector composed of nRF52840-DK and EVB3000 boards.
"""
import threading
import datetime
from time import sleep, time
import datetime
import serial

class CirCollector(threading.Thread):
    """
    A CIR getter thread gets CIRs from the sensor and puts them into a FIFO queue
    """

    def __init__(self, port, stop_criterion, cir_file):
        """CirCollector constructor

        Parameters
        ----------
        port : str
            nRF52840 DK (SEGGER J-Link) port
        serial_speed : int
            serial speed
        cir_file : File
            file for CIR saving
        stop_criterion : threading.Event
            stop flag
        """
        super().__init__()
        self.counter = 0
        self.ser = port
        self.stop_criterion = stop_criterion
        self.cir_file=cir_file

    def run(self):
        """
        Start CIR collector thread.
        """

        # flush the UART and read first batch and discard it (for safety).
        self.ser.flush()

        cir_time=0
        while True:

            if self.stop_criterion.is_set():
                # close UART
                # self.ser.close()
                print("gathered {} CIRs".format(self.counter))
                break

            # # sleep for a while to let the module output all data to UART
            # sleep(0.1)

            # read until the ending bytes "[OK]"
            s = self.ser.read_until(b"[OK]")

            # get current timestamp
            cir_time = datetime.datetime.now().timestamp()

            if len(s):
                # store the results in the file
                self.cir_file.write(b"%.6f" % cir_time)
                self.cir_file.write(s)
                self.cir_file.flush()

                self.counter = self.counter + 1


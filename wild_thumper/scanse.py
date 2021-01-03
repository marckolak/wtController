"""
The scanse module contains functions for interaction with the scanse scanner.
"""


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

    # Iterate over an infinite scan generator. Stop when asked to do so
    def run(self):
        """
        Start scanner thread. Puts the scans into a queue until the `stop_criterion` is achieved
        """
        with Sweep(self.dev) as sweep:
            sweep.start_scanning()

            for scan in sweep.get_scans():
                if self.counter == self.stop_criterion:
                    self.queue.put_nowait(None)
                    print("gathered {} scans".format(self.counter))
                    break
                else:
                    self.queue.put_nowait(scan)
                    self.counter += 1

            sweep.stop_scanning()


class ScanGetter(threading.Thread):
    """
    ScanGetter extracts scans from the referenced queue.
    """

    def __init__(self, queue):
        super().__init__()
        self.queue = queue
        self.scans = []



    def run(self):
        """
        Iterate over the queue's scans blocking if the queue is empty. If the value in queue is None leave thread.
        """
        while True:
            scan = self.queue.get()

            if not scan:
                break

            self.scans.append(scan)

            self.queue.task_done()

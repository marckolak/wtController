"""
The controller file contains the main script, which enables platform's work.
"""
import json
import socket
import sys
from sweeppy import Sweep

sys.path.append('../')

from wild_thumper.robot import Robot
from wild_thumper.smcg2 import SmcG2Serial, open_port
import serial


# global variable, which specifies the mode the controller works in - dummy mode allwos to communicate without peripherals
dummy_mode = False


class ControllerInitError(Exception):
    def __init__(self, message):
        super(ControllerInitError, self).__init__(message)


def init_socket(host, port):
    try:  # socket initialization
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.bind(('', port))
        print("Socket initialized at host {}, port {}".format(host, port))
        return s
    except socket.error as err:
        raise ControllerInitError(str(err))


def init_motor_controllers(mc_left_port, mc_right_port):
    try:  # initialize motor controllers
        mc_port_l = open_port(mc_left_port)
        mc_port_r = open_port(mc_right_port)

        smc_left = SmcG2Serial(mc_port_l, None)
        smc_left.exit_safe_start()
        smc_left.set_target_speed(0)
        print("Left SmcG2 error status: 0x{:04X}".format(smc_left.get_error_status()))

        smc_right = SmcG2Serial(mc_port_r, None)
        smc_right.exit_safe_start()
        smc_right.set_target_speed(0)
        print("Right SmcG2 error status: 0x{:04X}".format(smc_right.get_error_status()))

        return smc_left, smc_right

    except serial.SerialException as serr:
        if dummy_mode:
            return SmcG2Serial(serial.Serial(), None, dummy=True), SmcG2Serial(serial.Serial(), None, dummy=True)
        else:
            raise ControllerInitError("Motor controller error: {}".format(serr))


def init_scanner(scanner_port):
    # initialize LiDAR - set speed to 0 Hz
    try:
        with Sweep(scanner_port) as sweep:
            sweep.set_sample_rate(750)
            sweep.set_motor_speed(0)
    except RuntimeError as e:
        print('Scanner error: ' + str(e))
    return None


def process_message(data, conn, robot):
    try:
        message = json.loads(data)
        cmd = message['cmd']
        payload = message["payload"]
        if cmd == 'move':
            if payload['time'] <= 0:
                robot.move(payload["direction"], payload["speed"])
        if cmd == 'scan':
            robot.process_scan_message(payload)

    except json.decoder.JSONDecodeError:
        robot.move('stop', 0)  # stop the robot from moving
        print("JSON decoding error for data: ", data)


def main():
    print("Starting wild thumper controller")

    try:  # controller initialization and main loop

        # load settings
        try:
            with open('controller_settings.json') as f:
                settings = json.load(f)

            global dummy_mode
            dummy_mode = settings['dummy']
            HOST = settings['host']  # localhost
            PORT = settings['port']  # port

            mc_left_port = settings['left_motor']
            mc_right_port = settings['right_motor']
            scanner_port = settings['scanner']
        except (FileNotFoundError, KeyError) as e:
            raise ControllerInitError('\033[91m' + 'Error while loading settings:  ' + str(e) + '\033[0m')

        # initialize peripherals
        s = init_socket(HOST, PORT)  # initialize socket
        smc_left, smc_right = init_motor_controllers(mc_left_port, mc_right_port)  # initialize motors
        init_scanner(scanner_port)  # initialize scanner

        # create robot instace
        robot = Robot(motor_left=smc_left, motor_right=smc_right, scanner=scanner_port)

        # MAIN LOOP
        # wait for uncoming packets
        while True:  
            print("Awaiting connection...")
            s.listen()
            conn, addr = s.accept()
            try:
                with conn:
                    print('Connected by', addr)
                    while True:
                        data = conn.recv(1024)
                        if not data:
                            print("Client disconnected. Connection {} lost".format(addr))
                            break

                        process_message(data, conn, robot)

            except socket.error as err:
                print("Socket error: ", err)
                print(type(err))

    except ControllerInitError as e:
        print(e)
        print("Closing the controller application...")


if __name__ == "__main__":
    main()

"""
The controller file contains the main script, which enables platform's work.
"""
import json
import socket

import serial

import sys
sys.path.append('../')

from robot import Robot
from smcg2 import SmcG2Serial, open_port

HOST = '192.168.0.200'  # localhost
PORT = 4312  # port

mc_left_port = "/dev/ttyLeftMotor"
mc_right_port = "/dev/ttyRightMotor"
scanner_port = "USB2"


class ControllerInitError(Exception):
    def __init__(self, message):
        super(ControllerInitError, self).__init__(message)


def init_socket(host, port):
    try:  # socket initialization
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.bind((host, port))
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
        print("Left SmcG2 error status: 0x{:04X}".format(smc_right.get_error_status()))

        return smc_left, smc_right

    except serial.SerialException as serr:
        raise ControllerInitError("Motor controller error: {}".format(serr))


def init_scanner(scanner_port):
    # initialize LiDAR
    scanner = None

    return scanner


def process_message(data, conn, robot):
    try:
        message = json.loads(data)
        cmd = message['cmd']
        payload = message["payload"]
        if cmd == 'move':
            if payload['time'] <= 0:
                robot.move(payload["direction"], payload["speed"])

    except json.decoder.JSONDecodeError:
        robot.move('stop', 0)  # stop the robot from moving
        print("JSON decoding error for data: ", data)


def main():
    print("Starting wild thumper controller")

    try:  # controller initialization and main loop

        s = init_socket(HOST, PORT)  # initialize socket
        smc_left, smc_right = init_motor_controllers(mc_left_port, mc_right_port)  # initialize motors
        scanner = init_scanner('')  # initialize scanner
        #smc_left, smc_right = None, None

        robot = Robot(motor_left=smc_left, motor_right=smc_right, scanner=scanner)

        while True:  # main program loop
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


if __name__ == "__main__":
    main()

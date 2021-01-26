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
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.bind(('', port))
        print("Socket initialized at host {}, port {}".format(host, port))
        return s

    except socket.error as err:
        raise ControllerInitError(str(err))


def init_motor_controllers(mc_left_port, mc_right_port, status_dict):
    try:  # initialize motor controllers
        
        mc_port_l = open_port(mc_left_port)
        mc_port_r = open_port(mc_right_port)

        # init left controller
        smc_left = SmcG2Serial(mc_port_l, None)
        smc_left.exit_safe_start()
        smc_left.set_target_speed(0)
        error_status_l = smc_left.get_error_status()
        status_dict['left_motor'] = "0x{:04X}".format(error_status_l)
        print("Left SmcG2 error status: 0x{:04X}".format(error_status_l))
        
        # init right controller
        smc_right = SmcG2Serial(mc_port_r, None)
        smc_right.exit_safe_start()
        smc_right.set_target_speed(0)
        error_status_r = smc_right.get_error_status()
        status_dict['right_motor'] = "0x{:04X}".format(error_status_r)
        print("Right SmcG2 error status: 0x{:04X}".format(error_status_r))
        
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


def process_message(data, addr, robot):
    try:
        # load json object from message
        message = json.loads(data)
        # get command and payload
        
        cmd = message['cmd']
        payload = message["payload"]

        # pass to other functions
        if cmd == 'move': # move the platform
            if payload['time'] <= 0:  # control in 'real time' - sets speed with no time constraints
                robot.move(payload["direction"], payload["speed"])

        elif cmd == 'scan':  # start/stop LiDAR scanning
            robot.process_scan_message(payload)

        elif cmd == 'connect':  # start sending data to a client
            robot.connect((addr[0], payload['port']))

        elif cmd == 'disconnect':  # stop sending data to a client
            robot.disconnect()

        elif cmd =='status':  # send status data to the connected client
            robot.send_status()


    except json.decoder.JSONDecodeError:
        robot.move('stop', 0)  # stop the robot from moving
        print("JSON decoding error for data: ", data)


def main():
    print("Starting wild thumper controller")

    # init status dictionary
    status_dict = {'left_motor': '404', 'right_motor': '404', 'scanner': '404'}
    

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

        except (FileNotFoundError, KeyError) as e:  # don't start the platform without settings file
            raise ControllerInitError('\033[91m' + 'Error while loading settings:  ' + str(e) + '\033[0m')

        # initialize peripherals
        s = init_socket(HOST, PORT)  # initialize socket
        smc_left, smc_right = init_motor_controllers(mc_left_port, mc_right_port, status_dict)  # initialize motors
        init_scanner(scanner_port)  # initialize scanner

        # create robot instace
        robot = Robot(motor_left=smc_left, motor_right=smc_right, scanner=scanner_port, status_dict=status_dict)

        # MAIN LOOP
        # wait for uncoming packets
        while True:  
            print("Awaiting data...")
            
            try:
                # with conn:
                while True:
                    data, addr = s.recvfrom(1024)
                    process_message(data, addr, robot)

                    if not data:
                        print(" Connection with {} lost".format(addr))
                        break
                        
            except socket.error as err:
                print("Socket error: ", err)
                print(type(err))

    except ControllerInitError as e:  # in case of a serious error, without which it is not worth it to start the platform
        print(e)
        print("Closing the controller application...")


if __name__ == "__main__":
    main()

import argparse
import command
import mqtt_comm as mc
from threading import Thread
import time
import pygame
import os
import mqtt_state as ms
import cv2

import gzl.logger as gzlog
import gzl.cameras as gzcams

# Setup for options
parser = argparse.ArgumentParser(description='Client control application')
parser.add_argument('-d', '--debug', action='store_true',
                    help='Enable debug messages')
args = parser.parse_args()

# MAIN for gamepad+computer
SERVER_IP = '127.0.0.1'
# SERVER_IP = '10.139.1.134'


def setup_cameras_controller(logger=None):
    """ Setup camera controller with routes

            Returns:
                    GstCamerasController -- cameras controller
    """
    cam_params = [
        [gzcams.GstH264TCPClient, {'ip': SERVER_IP, 'port': 4000}],
        [gzcams.GstH264TCPClient, {'ip': SERVER_IP, 'port': 5000}],
    ]

    cams_controller = gzcams.GstCamerasController(cam_params, logger)

    return cams_controller


def setup_logger():
    """ Setup logger with filename same as module name 
            return logger from logging module 
            (https://docs.python.org/3/library/logging.html)

            Returns:
                    Logger -- Python object of module logging
    """
    basename = os.path.basename(__file__).split('.')[0]
    gzlog.setup_logger(basename + '.log')
    logger = gzlog.get_logger()

    return logger


rootLogger = setup_logger()
cams_controller = setup_cameras_controller(rootLogger)

# Must be after setup_logger()
if args.debug:
    gzlog.enable_debug()

pygame.init()
hrz_value_delay = 0
vrt_value_delay = 0
try:
    joystick = pygame.joystick.Joystick(0)
    joystick.init()
    rootLogger.debug("joystick initialized")
    joy_vrt_axis = 4
    joy_hrz_axis = 0

except:
    joystick = None
    rootLogger.error('Connect your joystick!')
    exit()

json_str = None
pub = command.CommandPub(ip=SERVER_IP)
isActive = True
last_values = (0, 0)


def sender_thread():
    while isActive:
        time.sleep(1)
        pub.send_set(*last_values)


Thread(target=sender_thread).start()


def state_on_msg(data):
    rootLogger.info(data.decode('UTF-8'))


state_sub = ms.MQTTStateSub(ip=SERVER_IP, on_msg=state_on_msg)

try:
    vrt, hrz = 0, 0
    rootLogger.debug("start")
    while isActive:
        # CAMERAS ROUTINE
        if cams_controller.is_connected():
            frame = cams_controller.read_frame()
            if frame is not None:
                cv2.imshow('view', frame)
                cv2.waitKey(1)

        # JOYSTICK ROUTINE
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                isActive = False
            if joystick is not None:
                if event.type == pygame.JOYAXISMOTION:
                    if event.axis == joy_hrz_axis:
                        if abs(event.value) > 0.02:
                            hrz = int(event.value * 100)
                        else:
                            hrz = 0
                    elif event.axis == joy_vrt_axis:
                        if abs(event.value) > 0.02:
                            # 3.99 selected to disable possible value 20/-20
                            vrt = int(event.value * -3.99) * 5
                        else:
                            vrt = 0

                    if (vrt, hrz) != last_values:
                        last_values = (vrt, hrz)
                        pub.send_set(vrt, hrz)
                        rootLogger.debug(
                            'send_enable: vrt = {}, hrz = {}'.format(vrt, hrz))

                if event.type == pygame.JOYBUTTONDOWN:
                    # LB
                    if event.button == 4:
                        pub.send_enable()
                        rootLogger.debug('send_enable')
                    # RB
                    if event.button == 5:
                        pub.send_disable()
                        rootLogger.debug('send_disable')
                    # X / blue
                    if event.button == 2:
                        pub.send_start()
                        rootLogger.debug('send_start')
                    # B / red
                    if event.button == 1:
                        pub.send_stop()
                        rootLogger.debug('send_stop')
                    # Y / yellow
                    if event.button == 3:
                        cams_controller.switch_cam()
                        rootLogger.debug('switch camera')
                    # A / green
                    if event.button == 0:
                        rootLogger.debug('not used / green')

finally:
    pygame.quit()
    rootLogger.debug("exit")
    isActive = False

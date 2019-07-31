import communication
import time
import command
import os
import mqtt_state as ms

import gzl.logger as gzlog

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

# Setup for options
import argparse
parser = argparse.ArgumentParser(description='Server control application')
parser.add_argument('-d', '--debug', action='store_true', help='Enable debug messages')
args = parser.parse_args()



# MAIN for gazel computer

BROCKER_IP = '127.0.0.1'
STM_COMMUNICATION_DEVICE='/dev/serial/by-id/usb-STMicroelectronics_ChibiOS_RT_Virtual_COM_Port_404-if00'
STM_COMMUNICATION_DEVICE='/dev/ttyACM1'

rootLogger = setup_logger()
if args.debug:
	gzlog.enable_debug()

rootLogger.debug('start')

state_pub = ms.MQTTStatePub(ip = BROCKER_IP)

try:
    comun = communication.CommunicationOnSerial(STM_COMMUNICATION_DEVICE)
    comun.enable_debugging()
    comun.activate_connection()
except:
    comun = None
    rootLogger.warning('Serial is disabled! Work only for input testing.')

sub = command.CommandSub(ip = BROCKER_IP)
print('START SUB')
def set_handler(load):
    print("get set from teleop")
    if comun:
        comun.set_control(load['speed'], load['steer'],)
    rootLogger.debug('COMM_SET: speed = {}, steer = {}'.format(load['speed'], load['steer']))
def start_handler():
    print("get start from teleop")
    if comun:
        comun.on_start()
    rootLogger.debug("COMM_START")
def stop_handler():
    print("get stop from teleop")
    if comun:
        comun.on_stop()
    rootLogger.debug("COMM_STOP")
def steer_handler():
    print("get steer from teleop")
    if comun:
        comun.on_steer()
    rootLogger.debug("COMM_STOP")
# NEW START
def steer2_handler():
    print("get steer2 from teleop")
    if comun:
        comun.on_steer()
    rootLogger.debug("COMM_STOP")
# NEW END
def enable_handler():
    print("get enable from teleop")
    if comun:
        comun.activate_connection()
    rootLogger.debug("COMM_ENABLE")
def disable_handler():
    print("get disable from teleop")
    if comun:
        comun.deactivate_connection()
    rootLogger.debug("COMM_DISABLE")

sub.on_set = set_handler
sub.on_start = start_handler
sub.on_stop = stop_handler
sub.on_enable = enable_handler
sub.on_disable = disable_handler
sub.on_steer = steer_handler
# NEW START
sub.on_steer2 = steer2_handler
# NEW END

while True:	
    # state_pub.send("Hello from the GAZEL's side")
    if comun:
        inp = comun.get_state_msg()
        
        # print("inp = {}".format(inp))
        if inp:
            rootLogger.debug('Debug output: {} (lvl: {})'.format(inp.msg, inp.lvl))
            
            if inp.lvl == communication.StateMessage.INFO_LVL or \
                inp.lvl == communication.StateMessage.UNKNOWN_LVL:
                rootLogger.info('From uC: {}'.format(inp.msg))
                state_pub.send(inp.msg)

            if inp.lvl == communication.StateMessage.WARNING_LVL:
                rootLogger.warning('From uC: {}'.format(inp.msg))
                state_pub.send(inp.msg)

            if inp.lvl == communication.StateMessage.ERROR_LVL:
                rootLogger.error('From uC: {}'.format(inp.msg))
                state_pub.send(inp.msg)

    time.sleep(0.001)

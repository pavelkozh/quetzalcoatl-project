import communication
import time
import command
import os

# Setup for logging
import gzl.logger as gzlog
basename = os.path.basename(__file__).split('.')[0]
gzlog.setup_logger(basename + '.log')
rootLogger = gzlog.get_logger()

# Setup for options
import argparse
parser = argparse.ArgumentParser(description='Server control application')
parser.add_argument('-d', '--debug', action='store_true', help='Enable debug messages')
args = parser.parse_args()

if args.debug:
	gzlog.enable_debug()

# MAIN for gazel computer

BROCKER_IP = '127.0.0.1'
STM_COMMUNICATION_DEVICE='/dev/serial/by-id/usb-STMicroelectronics_ChibiOS_RT_Virtual_COM_Port_404-if00'

rootLogger.debug('start')

try:
    comun = communication.CommunicationOnSerial(STM_COMMUNICATION_DEVICE)
    comun.enable_debugging()
    comun.activate_connection()
except:
    comun = None
    rootLogger.warning('Serial is disabled! Work only for input testing.')

sub = command.CommandSub(ip = BROCKER_IP)
def set_handler(load):
    if comun:
        comun.set_control(load['speed'], load['steer'],)
    rootLogger.debug('COMM_SET: speed = {}, steer = {}'.format(load['speed'], load['steer']))
def start_handler():
    if comun:
        comun.on_start()
    rootLogger.debug("COMM_START")
def stop_handler():
    if comun:
        comun.on_stop()
    rootLogger.debug("COMM_STOP")
def enable_handler():
    if comun:
        comun.activate_connection()
    rootLogger.debug("COMM_ENABLE")
def disable_handler():
    if comun:
        comun.deactivate_connection()
    rootLogger.debug("COMM_DISABLE")

sub.on_set = set_handler
sub.on_start = start_handler
sub.on_stop = stop_handler
sub.on_enable = enable_handler
sub.on_disable = disable_handler

while True:	
    if comun:
        inp = comun.get_state_msg()
        if inp:
            rootLogger.debug('Debug output: {} (lvl: {})'.format(inp.msg, inp.lvl))
            
            if inp.lvl == communication.StateMessage.INFO_LVL or \
                inp.lvl == communication.StateMessage.UNKNOWN_LVL:
                rootLogger.info('From uC: {}'.format(inp.msg))

            if inp.lvl == communication.StateMessage.WARNING_LVL:
                rootLogger.warning('From uC: {}'.format(inp.msg))

            if inp.lvl == communication.StateMessage.ERROR_LVL:
                rootLogger.error('From uC: {}'.format(inp.msg))

    time.sleep(0.001)

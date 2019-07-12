import communication
import time
import command

# MAIN for gazel computer

BROCKER_IP = '127.0.0.1'
STM_COMMUNICATION_DEVICE='/dev/serial/by-id/usb-STMicroelectronics_ChibiOS_RT_Virtual_COM_Port_404-if00'

print('start')

try:
    comun = communication.CommunicationOnSerial(STM_COMMUNICATION_DEVICE)
    comun.enable_debugging()
    comun.activate_connection()
except:
    comun = None
    print('Serial is disabled! Work only for input testing.')

sub = command.CommandSub(ip = BROCKER_IP)
def set_handler(load):
    if comun:
        comun.set_control(load['speed'], load['steer'],)
    print('COMM_SET: speed = {}, steer = {}'.format(load['speed'], load['steer']))
def start_handler():
    if comun:
        comun.on_start()
    print("COMM_START")
def stop_handler():
    if comun:
        comun.on_stop()
    print("COMM_STOP")
def enable_handler():
    if comun:
        comun.activate_connection()
    print("COMM_ENABLE")
def disable_handler():
    if comun:
        comun.deactivate_connection()
    print("COMM_DISABLE")

sub.on_set = set_handler
sub.on_start = start_handler
sub.on_stop = stop_handler
sub.on_enable = enable_handler
sub.on_disable = disable_handler

while True:	
    if comun:
        inp = comun.get_state_msg()
        if inp:
            print('Debug output: {}'.format(inp.msg))

    time.sleep(0.001)

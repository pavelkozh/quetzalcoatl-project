import communication
import mqtt_comm as mc
import time
import command
# MAIN for gazel computer

BROCKER_IP = '127.0.0.1'
STM_COMMUNICATION_DEVICE='/dev/serial/by-id/usb-STMicroelectronics_ChibiOS_RT_Virtual_COM_Port_404-if00'

print('start')
comun = communication.CommunicationOnSerial(STM_COMMUNICATION_DEVICE)
comun.enable_debugging()
comun.activate_connection()

sub = command.CommandSub(ip = BROCKER_IP)
def set_handler(load):
    comun.set_control(load['speed'], load['steer'],)
    print('COMM_SET: speed = {}, steer = {}'.format(load['speed'], load['steer']))
def start_handler():
    comun.on_start()
    print("COMM_START")
def stop_handler():
    comun.on_stop()
    print("COMM_STOP")
def enable_handler():
    comun.activate_connection()
    print("COMM_ENABLE")
def disable_handler():
    comun.deactivate_connection()
    print("COMM_DISABLE")
sub.on_set = set_handler
sub.on_start = start_handler
sub.on_stop = stop_handler
sub.on_enable = enable_handler
sub.on_disable = disable_handler
test_work = True
try:
    while test_work:	
        inp = comun.get_debug_line()
        if inp:
            print('Debug output: {}'.format(inp))

        time.sleep(0.001)
finally:
    test_work = False

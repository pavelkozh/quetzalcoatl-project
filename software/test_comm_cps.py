import communication
import time
import timeit

STM_COMMUNICATION_DEVICE='/dev/serial/by-id/usb-STMicroelectronics_ChibiOS_RT_Virtual_COM_Port_404-if00'
# STM_COMMUNICATION_DEVICE='/dev/ttyACM0'

comun = communication.CommunicationOnSerial(STM_COMMUNICATION_DEVICE)
comun.enable_debugging()
comun.activate_connection()

print('Start')

def test_start_cmd():
    comun.on_start()

count = 10
time = timeit.timeit(test_start_cmd, number=count)
print(time/count)

# for i in range(10):
#     test_start_cmd()
    
#     inp = comun.get_debug_line()
#     if inp:
#         print('Debug output: {}'.format(inp))

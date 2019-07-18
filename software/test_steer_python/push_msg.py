import communication

import argparse

import serial
import numpy as np
import time
import sys
# Производится чтение аргумента из консоли.
parser = argparse.ArgumentParser(description='Communication script test')
parser.add_argument('device', action='store', help='Serial port device')

args = parser.parse_args()

# Создание объекта и передача полученного аргумента из консоли.
Connection = communication.CommunicationOnSerial(args.device)

print('Set connection activated')
Connection.activate_connection()
print('Set debug enabled')
Connection.enable_debugging()

print('Start main loop')

check_time = time.time()


#  Для запуска в консоли  " python push_msg.py /dev/ttyACM1 " 
# в зависимости от операционной системы проверить к какому порту подключена STM
# для linux  это /dev/ttyACM1
# pip install -r requirements.txt
while(1):

    if time.time() - check_time > 1:
        check_time = time.time()
        speed = input('Print speed: ')
        # speed, angle = input("Put speed, angle ").split()
        # Connection.set_control(speed, angle)
        if speed == 'start':
            Connection.on_start()

        elif speed == 'steer':
            Connection.on_stop()

        else:
            spst_pair = (int(speed), 0)
            print('New speed/steer pair: {}'.format(spst_pair))

            Connection.set_control(spst_pair[0], spst_pair[1])




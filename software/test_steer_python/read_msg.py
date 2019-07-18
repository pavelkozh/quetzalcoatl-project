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

#  Для запуска в консоли  "python read_msg.py /dev/ttyACM1"
# в зависимости от операционной системы проверить к какому порту подключена STM
# для linux  это /dev/ttyACM1

while(1):


    inp = Connection.get_state_msg()
    if inp:
        print('I get: {} ({})'.format(inp.msg, inp.lvl))


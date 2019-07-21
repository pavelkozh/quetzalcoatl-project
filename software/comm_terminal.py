import serial
import numpy as np
import time
import sys
import communication as cm

if __name__ == "__main__":

    import argparse

    # Производится чтение аргумента из консоли.
    parser = argparse.ArgumentParser(description='Communication script test')
    parser.add_argument('device', action='store', help='Serial port device')
    args = parser.parse_args()

    Connection = cm.CommunicationOnSerial(args.device)

    while(1):
        inp = Connection.get_state_msg()
        if inp:
            print(inp.msg)

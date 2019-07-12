#for teleops
import paho.mqtt.client as mqtt
import json
import time
import curses

CONST_VEL_STEP = 1
CONST_VEL_MIN = -100
CONST_VEL_MAX = 100

CONST_TURN_STEP = 1
CONST_TURN_MIN = -100
CONST_TURN_MAX = 100

CONST_STOP = 10
CONST_PAUSE = 32

CONST_W = 119
CONST_A = 97
CONST_S = 115
CONST_D = 100
CONST_UP = 259
CONST_DOWN = 258
CONST_LEFT = 260
CONST_RIGHT = 261

vel = 0
turn = 0
stop = 0
pause = 0

def type_teleop(stdscr):
    # do not wait for input when calling getch
    stdscr.nodelay(1)
    while rc == 0:
        # get keyboard input, returns -1 if none available
        c = stdscr.getch()
        if c != -1:
            # print numeric value
            stdscr.addstr(str(c) + ' ')
            stdscr.refresh()
            # return curser to start position
            stdscr.move(0, 0)
            pub.send_data(c)

class Talker(object):

    def __init__(self, ip):
        self.t_client = mqtt.Client()
        self.t_client.connect(host=ip, port=1883)
        self.vel = 0
        self.turn = 0
        self.stop = 0
        self.pause = 0

    def send_data(self, value):
        topic = 'teleops'
        # velocity 
        if value == CONST_W or value == CONST_UP:
            if (self.vel + CONST_VEL_STEP > CONST_VEL_MAX):
                self.vel = CONST_VEL_MAX
            else:
                self.vel = self.vel + CONST_VEL_STEP
        if value == CONST_S or value == CONST_DOWN:
            if (self.vel - CONST_VEL_STEP < CONST_VEL_MIN):
                self.vel = CONST_VEL_MIN
            else:
                self.vel = self.vel - CONST_VEL_STEP
        # angle
        if value == CONST_A or value == CONST_LEFT:
            if (self.turn + CONST_TURN_STEP > CONST_TURN_MAX):
                self.turn = CONST_TURN_MAX
            else:
                self.turn = self.turn + CONST_TURN_STEP
        if value == CONST_D or value == CONST_RIGHT:
            if (self.turn - CONST_TURN_STEP < CONST_TURN_MIN):
                self.turn = CONST_TURN_MIN
            else:
                self.turn = self.turn - CONST_TURN_STEP
        # pause
        if value == CONST_PAUSE:
            self.pause = 1
            self.stop = 0
            self.vel = 0
            #self.turn = 0
        # stop
        if value == CONST_STOP:
            self.stop = 1
            self.vel = 0
            self.pause = 0
            #self.turn = 0

        data = {"vel": self.vel, "turn" : self.turn, "pause" : self.pause, "stop" : self.stop}
        self.t_client.publish(topic, json.dumps(data), qos=0)


if __name__ == "__main__":
    # vel = 0
    # turn = 0
    # stop = 0
    # pause = 0
    pub = Talker('127.0.0.1')
    rc = 0
    curses.wrapper(type_teleop)
    
    # Continue the network loop, exit when an error occurs
rc = pub.t_client.loop()
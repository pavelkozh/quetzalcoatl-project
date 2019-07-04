#for teleops
import paho.mqtt.client as mqtt
import json 

# CONST_W = 119
# CONST_A = 97
# CONST_S = 115
# CONST_D = 100
# CONST_UP = 259
# CONST_DOWN = 258
# CONST_LEFT = 260
# CONST_RIGHT = 261

def on_message(client, userdata, message):
    teleop_comands = json.loads(message.payload)
    print(teleop_comands)
    vel = teleop_comands['vel']
    turn = teleop_comands['turn']
    stop = teleop_comands['stop']
    pause = teleop_comands['pause']
    print("vel = ")
    print(vel)
    print("turn = ")
    print(turn)
    print("pause = ")
    print(pause)
    print("stop = ")
    print(stop)

class Listener(object):

    def __init__(self, ip):
        self.l_client = mqtt.Client()

        self.l_client.connect(host=ip, port=1355)

        topic = 'teleops'
        self.l_client.on_message = on_message
        self.l_client.subscribe(topic) #, qos=0)

if __name__ == "__main__":
    sub = Listener('127.0.0.1')

    sub.l_client.loop_forever()
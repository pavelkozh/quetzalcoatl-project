import paho.mqtt.client as mqtt
from threading import Thread
import time


class MQTTControl(object):
    TOPIC_NAME = 'gaz/control'
    TOPIC_QOS = 0


class MQTTControlPub(object):
    def __init__(self, ip='127.0.0.1'):
        self.client = mqtt.Client(client_id='control_pub')
        self.client.connect(host=ip, port=1883, keepalive=5)

        self.client.on_disconnect = self._disc_hdlr

    # Couldn`t call this, need to check if it is called after disconnect
    def _disc_hdlr(self):
        print('>>> Disconnect!')

    def send(self, json_str):
        self.client.publish(
            payload=json_str,
            topic=MQTTControl.TOPIC_NAME,
            qos=MQTTControl.TOPIC_QOS
        )

    def __del__(self):
        self.client.disconnect()


class MQTTControlSub(object):
    def __init__(self, ip='127.0.0.1', on_msg=None):
        self.client = mqtt.Client(client_id='control_sub')
        self.client.connect(host=ip, port=1883)

        self.on_msg = on_msg

        self.client.on_message = self._msg_handler
        self.client.subscribe(
            topic=MQTTControl.TOPIC_NAME,
            qos=MQTTControl.TOPIC_QOS
        )

        self.client.loop_start()
    
    # Proxy for converting arguments
    def _msg_handler(self, client, userdata, message):
        if self.on_msg:
            self.on_msg(message.payload)

    def __del__(self):
        self.client.disconnect()
        self.client.loop_stop()

if __name__ == "__main__":
    def on_msg(data):
        print(data)

    test_work = True

    def thread_pub(value):
        pub = MQTTControlPub()

        while test_work:
            # print('send')
            pub.send(value)
            time.sleep(1)

    Thread(target=thread_pub, args=(5,)).start()

    sub = MQTTControlSub(on_msg=on_msg)

    try:
        while test_work:
            # print('wait')
            time.sleep(1)
    finally:
        test_work = False

import paho.mqtt.client as mqtt
from threading import Thread
import time
import queue

class MQTTState(object):
    TOPIC_NAME = 'gaz/state'
    TOPIC_QOS = 0

class MQTTStatePub(object):
    def __init__(self, ip='127.0.0.1'):
        self.client = mqtt.Client(client_id='state_pub')
        self.client.connect(host=ip, port=1883, keepalive=5)

        self.client.on_disconnect = self._disc_hdlr

    # Couldn`t call this, need to check if it is called after disconnect
    def _disc_hdlr(self):
        print('>>> Disconnect!')

    def send(self, str_):
        self.client.publish(
            payload=str_,
            topic=MQTTState.TOPIC_NAME,
            qos=MQTTState.TOPIC_QOS
        )

    def __del__(self):
        self.client.disconnect()


class MQTTStateSub(object):
    def __init__(self, ip='127.0.0.1', on_msg=None):
        self.client = mqtt.Client(client_id='state_sub')
        self.client.connect(host=ip, port=1883)

        self.on_msg = on_msg
        self.message_queue = queue.Queue()

        self.client.on_message = self._msg_handler
        self.client.subscribe(
            topic=MQTTState.TOPIC_NAME,
            qos=MQTTState.TOPIC_QOS
        )

        # Thread(target=self._queue_message_handler).start()
        self.client.loop_start()
    
    # Proxy for converting arguments
    def _msg_handler(self, client, userdata, message):
        # self.message_queue.put(message.payload)
        if self.on_msg:
            self.on_msg(message.payload)

    def _queue_message_handler(self):
        while True:
            message = self.message_queue.get()
            if self.on_msg:
                self.on_msg(message)

    def __del__(self):
        self.client.disconnect()
        self.client.loop_stop()

if __name__ == "__main__":
    def on_msg(data):
        print('received: {}'.format(data))


    def thread_pub():
        pub = MQTTStatePub()

        counter = 0
        while test_work:
            str_ = 'some string: ' + str(counter)
            pub.send(str_)
            print('sent ' + str_)
            counter += 1
            time.sleep(1)

    test_work = True
    Thread(target=thread_pub).start()

    sub = MQTTStateSub(on_msg=on_msg)

    try:
        while test_work:
            # print('wait')
            time.sleep(1)
    finally:
        test_work = False
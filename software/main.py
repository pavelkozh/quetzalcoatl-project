#here will be Gazel's computer <---> Remote Control module AND Gazel's computer <---> STM32 module
import mqtt_teleop_subscriber as subscr

#maybe we should add an agrument for IP and port?
sub = subscr.Listener('127.0.0.1') 
sub.l_client.loop_forever()


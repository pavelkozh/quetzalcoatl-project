import communication
import mqtt_comm as mc
import time
import command
# MAIN for gazel computer

def on_msg(data):
	dictionary = command.CommandParse.parse(data)
	if dictionary['type'] == command.CommandTypes.COMM_ENABLE:
		comun.activate_connection()
		print("COMM_ENABLE")
	if dictionary['type'] == command.CommandTypes.COMM_DISABLE:
		comun.deactivate_connection()
		print("COMM_DISABLE")
	if dictionary['type'] == command.CommandTypes.COMM_START:
		comun.on_start()
		print("COMM_START")
	if dictionary['type'] == command.CommandTypes.COMM_STOP:
		comun.on_stop()
		print("COMM_STOP")
	if dictionary['type'] == command.CommandTypes.COMM_SET:
		print("COMM_SET")
		speed, angle = dictionary['value']
		comun.set_control(speed, angle)
		print(speed)
		print(angle)
test_work = True
sub = mc.MQTTControlSub(on_msg=on_msg)
comun = communication.CommunicationOnSerial('/dev/ttyACM0')
print('start')
try:
    while test_work:
        time.sleep(1)
finally:
    test_work = False

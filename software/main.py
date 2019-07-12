import command
import communication

# MAIN for gamepad+computer
#
# Get commands from gamepad
type_ = command.CommandTypes.COMM_SET
value = [-100, 100]

# Put data in json
json_str = command.CommandCreate.create(type_, value)
# send data by wifi 
#
#

# MAIN for gazel computer
#
# recieve data by wifi
#
#


# Parse command from gamepad
comun = communication.CommunicationOnSerial('/dev/ttyACM0')
dictionary = command.CommandParse.parse(json_str)
print(dictionary)
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
if dictionary['type'] == command.CommandTypes.COMM_SET:
	print("COMM_SET")
	speed, angle = dictionary['value']
	comun.set_control(speed, angle)
	print(speed)
	print(angle)


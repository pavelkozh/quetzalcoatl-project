import command
import mqtt_comm as mc
from threading import Thread
import time
import pygame
# MAIN for gamepad+computer
BROCKER_IP = '127.0.0.1'
pygame.init()
joystick_disabled = False
hrz_value_delay = 0
vrt_value_delay = 0
try:
	joystick = pygame.joystick.Joystick(0)
	joystick.init()
	print("init")
	joy_vrt_axis = 4
	joy_hrz_axis = 0

except:
	joystick_disabled = True
	print('Connect your joystick') 
	exit()

json_str = None
pub = command.CommandPub(ip = BROCKER_IP)
isActive = True
last_values = (0, 0)

def sender_thread():
	while isActive:
		time.sleep(1)
		pub.send_set(*last_values)

Thread(target=sender_thread).start()

try:
	prev_type_ = None
	vrt, hrz = 0, 0
	type_ = None
	print("start")
	while isActive:
		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				pygame.quit()
				isActive = False
			if not joystick_disabled:
				if event.type == pygame.JOYAXISMOTION:
					if event.axis == joy_hrz_axis:
						if abs(event.value) > 0.02:
							hrz = (int)(event.value * 100)
						else:
							hrz = 0
					elif event.axis == joy_vrt_axis:
						if abs(event.value) > 0.02:
							vrt = -1 * (int)(event.value * 100)
							if vrt == 0:
								vrt = 0
							elif vrt >= 1 and vrt <= 34:
								vrt = 5
							elif vrt >= 35 and vrt <= 65:
								vrt = 10
							elif vrt >= 66 and vrt <= 100:
								vrt = 15
							elif vrt <= -1 and vrt >= -34:
								vrt = -5
							elif vrt <= -35 and vrt >= -65:
								vrt = -10
							elif vrt <= -66 and vrt >= -100:
								vrt = -15
						else:
							vrt = 0
					last_values = (vrt, hrz)
					pub.send_set(vrt, hrz)
					print('send_enable: vrt = {}, hrz = {}'.format(vrt, hrz))

				if event.type == pygame.JOYBUTTONDOWN:
					if event.button == 4:
						pub.send_enable()
						print('send_enable')
					if event.button == 5:
						pub.send_disable()
						print('send_disable')
					if event.button == 2:
						pub.send_start()
						print('send_start')
					if event.button == 1:
						pub.send_stop()
						print('send_stop')
finally:
	pygame.quit()
	print("exit")
	isActive = False



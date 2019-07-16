import command
import mqtt_comm as mc
from threading import Thread
import time
import pygame
import os

# Setup for logging
import gzl.logger as gzlog
basename = os.path.basename(__file__).split('.')[0]
gzlog.setup_logger(basename + '.log')
rootLogger = gzlog.get_logger()

# Setup for options
import argparse
parser = argparse.ArgumentParser(description='Client control application')
parser.add_argument('-d', '--debug', action='store_true', help='Enable debug messages')
args = parser.parse_args()

if args.debug:
	gzlog.enable_debug()


# MAIN for gamepad+computer
BROCKER_IP = '127.0.0.1'
# BROCKER_IP = '10.139.1.134'

pygame.init()
joystick_disabled = False
hrz_value_delay = 0
vrt_value_delay = 0
try:
	joystick = pygame.joystick.Joystick(0)
	joystick.init()
	rootLogger.debug("init")
	joy_vrt_axis = 4
	joy_hrz_axis = 0

except:
	joystick_disabled = True
	rootLogger.error('Connect your joystick') 
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
	rootLogger.debug("start")
	while isActive:
		for event in pygame.event.get():
			if event.type == pygame.QUIT:
				pygame.quit()
				isActive = False
			if not joystick_disabled:
				if event.type == pygame.JOYAXISMOTION:
					if event.axis == joy_hrz_axis:
						if abs(event.value) > 0.02:
							hrz = int(event.value * 100)
						else:
							hrz = 0
					elif event.axis == joy_vrt_axis:
						if abs(event.value) > 0.02:
							# 3.99 selected to disable possible value 20/-20
							vrt = int(event.value * -3.99) * 5
						else:
							vrt = 0

					if (vrt, hrz) != last_values:
						last_values = (vrt, hrz)				
						pub.send_set(vrt, hrz)
						rootLogger.debug('send_enable: vrt = {}, hrz = {}'.format(vrt, hrz))

				if event.type == pygame.JOYBUTTONDOWN:
					if event.button == 4:
						pub.send_enable()
						rootLogger.debug('send_enable')
					if event.button == 5:
						pub.send_disable()
						rootLogger.debug('send_disable')
					if event.button == 2:
						pub.send_start()
						rootLogger.debug('send_start')
					if event.button == 1:
						pub.send_stop()
						rootLogger.debug('send_stop')
finally:
	pygame.quit()
	rootLogger.debug("exit")
	isActive = False



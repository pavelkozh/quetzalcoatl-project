import command
import mqtt_comm as mc
from threading import Thread
import time
import pygame
# MAIN for gamepad+computer

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
    print('except')
json_str = None
def thread_pub():
    pub = mc.MQTTControlPub()
    try:
    	isActive = True
    	prev_type_ = None
    	prev_vrt = None
    	prev_hrz = None
    	vrt, hrz = 0, 0
    	type_ = None
    	value = [None, None]
    	print("start")
    	while isActive:
    		for event in pygame.event.get():
	            if event.type == pygame.QUIT:
	                pygame.quit()
	                isActive = False
	            if not joystick_disabled:
	                if event.type == pygame.JOYAXISMOTION:
	                	type_ = command.CommandTypes.COMM_SET
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
	                	value = [vrt, hrz]
	                        
	                if event.type == pygame.JOYBUTTONDOWN:
	                    if event.button == 4:
	                        type_ = command.CommandTypes.COMM_ENABLE
	                        value = None
	                    if event.button == 5:
	                        type_ = command.CommandTypes.COMM_DISABLE
	                        value = None
	                    if event.button == 2:
	                        type_ = command.CommandTypes.COMM_START
	                        value = None
	                    if event.button == 1:
	                        type_ = command.CommandTypes.COMM_STOP
	                        value = None
	            if prev_type_ != type_:
	            	prev_type_ = type_
	            	json_str = command.CommandCreate.create(type_, value)
	            	print(json_str)
	            	pub.send(json_str)
	            else:
	            	if type_ == command.CommandTypes.COMM_SET:
	            		#Если тип SET - проверяем, что хотя бы одно из значений поменялось
	            		if value[0] != prev_vrt or value[1] != prev_hrz:
	            			prev_vrt = value[0]
	            			prev_hrz = value[1]
	            			json_str = command.CommandCreate.create(type_, value)
	            			print(json_str)
	            			pub.send(json_str)
    finally:
	    pygame.quit()
	    print("exit")
	    isActive = False

Thread(target=thread_pub).start()



import pygame
import math as m

pygame.init()
isActive = True
joystick_disabled = False
hrz_value_delay = 0
vrt_value_delay = 0
        
try:
    joystick = pygame.joystick.Joystick(0)
    joystick.init()

    joy_vrt_axis = 4
    joy_hrz_axis = 0

except:
    joystick_disabled = True
try:
    vrt, hrz = 0, 0
    while isActive:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                isActive = False
            if not joystick_disabled:
                # print(event.type)
                if event.type == pygame.JOYAXISMOTION:
                    # print("axis motion")
                    # print(event.axis)
                    if event.axis == joy_hrz_axis:
                        print(event.axis)
                        if abs(event.value) > 0.02: 
                            hrz = (int)(event.value * 100)
                        else:
                            hrz = 0

                    elif event.axis == joy_vrt_axis:
                        if abs(event.value) > 0.02:
                            vrt = (int)(event.value * 100)
                        else:
                            vrt = 0
                    
                    
                    elif event.axis == 2: #left
                        print("left value = {}".format(event.value * 100 + 101))
                    elif event.axis == 5: #right
                        print("right value = {}".format(event.value * 100 + 101))
                    print(vrt, hrz)   
                if event.type == pygame.JOYBUTTONDOWN:
                    print(event.button)
                    if event.button == 4:
                        print('Enable')
                    if event.button == 5:
                        print('Disable')
                    if event.button == 2:
                        print('Start')
                    if event.button == 1:
                        print('Stop')

finally:
    pygame.quit()
    isActive = False




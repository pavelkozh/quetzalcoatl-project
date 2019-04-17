#ifndef INCLUDE_LLD_CONTROL_H
#define INCLUDE_LLD_CONTROL_H

#include <common.h>

/*
 * Hardware description
 * ------------------------------------
 *
 * EXT driver uses inputs PE4, PE5
 * PWM (pin PE9) uses TIM1
 * PE11, PF14 uses as general outputs.
 */
/******************/
/*** Define ***/
/******************/



//PWM define
#define DRIVE_PWMD 						&PWMD3 		// PWM Driver
#define DRIVE_PWM_FREQ      			50000000 	// PWM clock frequency [Hz]
#define DRIVE_PWM_PERIOD				500   		// PWM period[tics]  
#define DRIVE_PWM_PIN					6			// pwm pin
#define DRIVE_PWM_PORT					GPIOC		// pwm pin port
#define DRIVE_PWM_PAL_MODE_ALTERNATE	2			// Alternate mod ffor pwm pin
#define DRIVE_GPTD						&GPTD5		// Driver for count pwm pulse
#define DRIVE_GPTD_TIM_REG_SMCR			TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1 | TIM_SMCR_SMS_2 | TIM_SMCR_TS_0 //Settings for Slave mode PWMD

//Drive
#define DRIVE_INVERT_ROTATION_DIRECTION		FALSE		// Invert rotation direction 
#define DRIVE_DIRECTION_PIN 				8			// Pin of set directions 
#define DRIVE_DIRECTION_PIN_PORT			GPIOB		// Pin port set directions


//Limit Switch
#define DRIVE_LIMIT_SWITCH_USE              FALSE
#define DRIVE_LIMIT_SWITCH_1_PIN            9
#define DRIVE_LIMIT_SWITCH_1_PIN_PORT       GPIOB
#define DRIVE_LIMIT_SWITCH_1_EXT_MODE_GPIO  EXT_MODE_GPIOB
#define DRIVE_LIMIT_SWITCH_2_PIN            15
#define DRIVE_LIMIT_SWITCH_2_PIN_PORT       GPIOB
#define DRIVE_LIMIT_SWITCH_2_EXT_MODE_GPIO  EXT_MODE_GPIOB


// Motor define
#define DRIVE_MAX_ANG                   160000      // 40 revolution or 20cm (5mm per revolution)
#define DRIVE_MAX_SPEED                 100000      // 100Khz or 1500 rpm
#define DRIVE_MIN_PULSE_WIDTH           (uint32_t)DRIVE_PWM_FREQ / DRIVE_MAX_SPEED

//Desired pwm impulses quantity (send to driver)
#define DRIVE_PWM_IMPULSE_QUANTITY    4000 // 200 for stepper motor




/******************/
/*** Prototypes ***/
/******************/



/*
 * @brief: lld control module initialization
 *         EXT driver initialization (buttons enable/disable, start/stop,
 *                                    left/right direction)
 *         setup PUL, DIR, EN control pins
 */
void drivelldControlInit( void );


/*
 * @brief:   enable PWM channel if enable button pressed
 * @note :   duty cycle always 50%
 * @param:   dir - direction
 *           ang - angle [pulse] (4000 pulse per revolution)
 */
void driveRun (bool dir, uint32_t ang, uint16_t speed);

/*
 * @brief :  set motor's rotation direction
 * @params:  dir - direction
 */
void driveRunSetDirection ( bool dir );

/**
 * @brief :  pwm has callback on pwm timer reset
 *           in collback function increments pwm pulses counter
 * @return : function return pwm pulses counter
 */
uint32_t drivegetPwmCounter ( void );

/**
 * @brief : function resets pwm pulses counter
 */
void driveresetPwmCounter ( void );

/*
 * @brief :  set motor's rotation speed
 * @params:  speed - speed of rotation (max speed - 100kHz)
 */
uint16_t driveSetSpeed(uint16_t speed);

/*
 * @brief :  drive stop
 */
void driveStop(void);

/*
 * @brief :  return drive state. 0 - drive stoped, 1 - drive run
 */
bool driveState();

#endif
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
/*** Prototypes ***/
/******************/

/* @brief: starts PWM Driver.
 *         Not enable. Enabling with certain duty cycle
 *         need to be done separately.
 */
void PWMUnitInit ( void );


/*
 * @brief: lld control module initialization
 *         EXT driver initialization (buttons enable/disable, start/stop,
 *                                    left/right direction)
 *         setup PUL, DIR, EN control pins
 */
void lldControlInit( void );


/*
 * @brief:    detect if driver enable or not (depends on buttons state)
 * @return:   true  - if enable and in start mode,
 *            false - if disable or in stop mode .
 */
bool ifDriverEnable ( void );


/*
 * @brief:   enable PWM channel if enable button pressed
 * @note :   duty cycle always 50%
 */
void motorRun ( void );


/*
 * @brief :    The function converts the motor speed specified as a percentage
 *             to the PWM period (number of ticks).
 * @params:    speed - may take values in the range (-100% - 100%);
 *             PWMConfig - PWM unit configuration structure
 * @return:    PWM period in ticks
 */
uint32_t mSpeed2pwmPeriodRecalc ( int8_t speed );


/*
 * @brief :   function change PWM period
 * @params:   speed - may take values in the range (-100% - 100%);
 *            PWMConfig - PWM unit configuration structure
 */
void motorSetSpeed ( int8_t speed );


/*
 *
 */
void setPwmPeriod ( uint16_t pwm_period_ticks );


#endif /* INCLUDE_LLD_CONTROL_H */

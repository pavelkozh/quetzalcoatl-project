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
 * @params:    v - may take values in the range ( 0%...100% );
 *             PWMConfig - PWM unit configuration structure
 * @return:    PWM period in ticks
 */
pwmcnt_t mSpeed2pwmPeriodRecalc ( uint8_t v );


/*
 * @brief : function set PWM period to control speed of rotation
 *          and set hardware pins which controls direction of rotation.
 * @params: speed in the range -100%...100% (sign specify direction)
 * @note  :
 */
void mSetSpeed ( int8_t speed );


/*
 * @brief :  directly change PWM period
 * @params:  desired PWM period in ticks
 */
void setPwmPeriod ( uint16_t pwm_period_ticks );


/*
 * @brief :  set motor's rotation direction
 * @params:  dir = 1 ->> clockwise rotation
 *           dir = 0 ->> counterclockwise rotation
 */
void mSetDirection ( bool dir );


#endif /* INCLUDE_LLD_CONTROL_H */

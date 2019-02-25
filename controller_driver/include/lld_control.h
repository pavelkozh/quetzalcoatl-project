#ifndef INCLUDE_LLD_CONTROL_H
#define INCLUDE_LLD_CONTROL_H

#include <common.h>

/*
 * Hardware description
 * ------------------------------------
 *
 * EXT driver uses inputs PE4, PE5
 * PWM PE9
 * PE11, PF14 uses as general outputs.
 */



/*** Prototypes ***/


/* @brief: starts PWM Driver.
 *         Not enable. Enabling with certain duty cycle
 *         need to be done separately.
 */
void PWMUnitInit ( void );


#endif /* INCLUDE_LLD_CONTROL_H */

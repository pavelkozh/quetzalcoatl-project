#ifndef INCLUDE_LLD_SENSOR_MODULE
#define INCLUDE_LLD_SENSOR_MODULE


#include <common.h>

/*
 * Hardware description
 * ------------------------------------
 * PC0, PA3 - ADC channels
 * TIM4 for ADC
 */



/*** Prototypes ***/

/* @brief: ADC initialization */
void commonADC1UnitInit ( void );


/* @brief:   ADC read channel
 * @prams:   Channel number from 1 (1,2..)
 * @return:  ADC value
 */
adcsample_t commonADC1UnitGetValue ( uint8_t ch );


#endif /* INCLUDE_LLD_SENSOR_MODULE */

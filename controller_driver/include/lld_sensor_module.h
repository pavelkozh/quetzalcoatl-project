#ifndef INCLUDE_LLD_SENSOR_MODULE
#define INCLUDE_LLD_SENSOR_MODULE


#include <common.h>


void commonADC1UnitInit ( void );
adcsample_t commonADC1UnitGetValue ( uint8_t ch );


#endif /* INCLUDE_LLD_SENSOR_MODULE */

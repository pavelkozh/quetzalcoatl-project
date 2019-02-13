#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_


#include <ch.h>
#include <hal.h>

#include <stdint.h>
#include <stdlib.h>

#define     PROGRAM_ROUTINE_MASTER                      0
#define     PROGRAM_ROUTINE_TEST_ADC                    1


#define     MAIN_PROGRAM_ROUTINE                        PROGRAM_ROUTINE_TEST_ADC

void commonADC1UnitInit ( void );
adcsample_t commonADC1UnitGetValue ( uint8_t ch );



#endif /* INCLUDE_COMMON_H_ */

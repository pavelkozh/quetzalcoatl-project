#ifndef INCLUDE_LLD_DAC_H_
#define INCLUDE_LLD_DAC_H_

#include <common.h>
#define REF_V				3.3
#define DAC_MAX				4095
#define VOLTAGE_2_DAC(v)  ((v) / REF_V * DAC_MAX)


void dac_init( void );

#endif //INCLUDE_LLD_DAC_H_

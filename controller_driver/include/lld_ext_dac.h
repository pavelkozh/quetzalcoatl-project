#ifndef INCLUDE_EXT_DAC_H_
#define INCLUDE_EXT_DAC_H_

#include <common.h>


/*
 * @brief: Init external Dac
 */

void extDacInit( void );

/*@brief: Set 16 bit value
*
*/
void  extDacSetValue( uint8_t valueA, uint8_t valueB);

#endif

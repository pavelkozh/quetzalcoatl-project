#ifndef INCLUDE_MT_CONTROL_H
#define INCLUDE_MT_CONTROL_H


#include <common.h>

void MTControlInit ( void );
void setGearBoxControlEnableFlag ( void );
void resetGearBoxControlEnableFlag ( void );
int8_t mannualyShiftGear ( uint8_t command_gear );
bool getGearBoxControlEnableFlag ( void );



#endif /* INCLUDE_MT_CONTROL_H */

#ifndef INCLUDE_MT_CONTROL_H
#define INCLUDE_MT_CONTROL_H


#include <common.h>

void mtControlInit ( void );
void mtControlStart ( void );
void mtControlStop( void );
int8_t mtControlGetCurrentGearNum (void);


/* Servis functions*/
bool mtControlGetEnableFlag ( void );
int8_t mtControlMannualyShiftGear ( uint8_t command_gear );






#endif /* INCLUDE_MT_CONTROL_H */
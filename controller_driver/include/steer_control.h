#ifndef INCLUDE_STEER_CONTROL_H
#define INCLUDE_STEER_CONTROL_H

#include <common.h>


void steerInit(void);
void steerControlStart(void);
void steerControlStop(void);
void steerSetPosition ( float val );
float steerGetPos( void );

int steerSyncTestDriver();

/***Debug**/
float steerDbgGetMotorCalcSpeedRef ( void );
float steerDbgGetMotorCalcPosErr ( void );
float steerDbgGetMotorPosRef ( void );
bool steerDbgGetEnableFlag ( void );

#endif /* INCLUDE_STEER_CONTROL_H */

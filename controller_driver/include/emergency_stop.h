#ifndef INCLUDE_EMERGENCY_STOP_H 
#define INCLUDE_EMERGENCY_STOP_H 

#include <common.h>
/*
 * @brief   Initialize pedals module, interrupt for emergency stop button and create thread for stop process
 */
void emergencyStopInit( void );
/*
 * @brief   Executes full stop WITHOUT swiching off. Can de called from any module
 */
bool emergencyFullStop (void) ;

#endif  /*INCLUDE_EMERGENCY_STOP_H */

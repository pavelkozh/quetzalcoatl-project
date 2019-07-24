#ifndef LLD_STEER_STEP_MOTOR_H
#define LLD_STEER_STEP_MOTOR_H


#include <common.h>


void lldSteerSMInit ( void );
int32_t lldSteerSMSetPosition (double position, uint16_t speed );
void lldSteerSMMove ( uint16_t speed );
void lldSteerSMStop ( void );
void lldSteerSMChangeSpeed ( uint16_t new_speed );
int32_t lldSteerSMGetPosition ( void );
bool lldSteerSMGetState ( void );
uint8_t lldSteerSMGetMode ( void );
uint16_t lldSteerSMGetSpeed ( void );
int32_t lldSteerSMGetMaxPosition ( void );
int32_t lldSteerSMGetMinPosition ( void );




#endif /* LLD_STEER_STEP_MOTOR_H */

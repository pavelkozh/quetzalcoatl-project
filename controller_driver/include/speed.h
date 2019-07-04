#ifndef INCLUDE_SPEED_H_
#define INCLUDE_SPEED_H_

#include <common.h>

/**
 * @brief     For synchronization
 * @param     engine_speed_rpm - reference [rpm]
 * @return    engine_control_value - GAS control [dac units]
 */
uint32_t sppedEngineControl( uint32_t engine_speed_rpm );

/**
 * @brief     Vehicle speed control function
 * @param     speed - reference [km/h]
 * @return    VehicleControl - GAS control [dac units]
 */
uint32_t sppedVehicleControl( uint32_t speed );

/**
 * @brief     Call PIDControlInit and start thread
 */
void speedInit ( void );

/**
 * @brief     Set the value of flag for the engine start control to true
 */
void speedEngineControlStart (void);

/**
 * @brief     Set the value of flag for the vehicle start control to true
 */
void speedVehicleControlStart(void);

/**
 * @brief     Set the value of flag for the engine start control to false
 */
void speedEngineControlStop(void);

/**
 * @brief     Set the value of flag for the vehicle start control to false
 */
void speedVehicleControlStop(void);


void speedSetVehiclePIDReferenceValue ( float val );
void speedSetEnginePIDReferenceValue ( float val );



double speedDbgGazelSpeed ( void );
double speedDbgGazelEngSpeed ( void );
int32_t speedDbgBrakePos ( void );
float speedGetPIDVal ( void );

float speedGetVehicleReference (void);
float speedGetEngineReference (void);


#endif /* INCLUDE_SPEED_H_ */

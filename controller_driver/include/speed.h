#ifndef INCLUDE_SPEED_H_
#define INCLUDE_SPEED_H_

#include <common.h>

/**
 * @brief     For synchronization
 * @param     engine_speed_rpm - reference [rpm]
 * @return    engine_control_value - GAS control [dac units]
 */
uint32_t sppedEngineSpeedControl( uint32_t engine_speed_rpm );

/**
 * @brief     Vehicle speed control function
 * @param     speed - reference [km/h]
 * @return    VehicleControl - GAS control [dac units]
 */
uint32_t sppedVehicleSpeedControl( uint32_t speed );

/**
 * @brief     Call PIDControlInit and start thread
 */
void speedPIDInit ( void );

/**
 * @brief     Set the value of flag for the engine start control to true
 */
void speedSetEngineControlStart (void);

/**
 * @brief     Set the value of flag for the vehicle start control to true
 */
void speedSetVehicleControlStart(void);

/**
 * @brief     Set the value of flag for the engine start control to false
 */
void speedResetEngineControlStart(void);

/**
 * @brief     Set the value of flag for the vehicle start control to false
 */
void speedResetVehicleControlStart(void);


void speedSetVehiclePIDReferenceValue ( float val );
void speedSetEnginePIDReferenceValue ( float val );



#endif /* INCLUDE_SPEED_H_ */

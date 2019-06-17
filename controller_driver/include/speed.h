#pragma once

/**
 * @brief     For synchronization
 * @param     engine_speed_rpm - reference [rpm]
 * @return    engine_control_value - GAS control [dac units]
 */
uint32_t engineSpeedControl( uint32_t engine_speed_rpm );

/**
 * @brief     Vehicle speed control function
 * @param     speed - reference [km/h]
 * @return    VehicleControl - GAS control [dac units]
 */
uint32_t vehicleSpeedControl( uint32_t speed );

/**
 * @brief     Call PIDControlInit and start thread
 */
void PIDInit ( void );

/**
 * @brief     Set the value of flag for the engine start control to true
 */
void setEngineControlStart (void);

/**
 * @brief     Set the value of flag for the vehicle start control to true
 */
void setVehicleControlStart(void);

/**
 * @brief     Set the value of flag for the engine start control to false
 */
void resetEngineControlStart(void);

/**
 * @brief     Set the value of flag for the vehicle start control to false
 */
void resetVehicleControlStart(void);


void setVehiclePIDReferenceValue ( float val );
void setEnginePIDReferenceValue ( float val );

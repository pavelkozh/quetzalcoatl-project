#ifndef INCLUDE_FEEDBACK_
#define INCLUDE_FEEDBACK_

#include <common.h>
#include <lld_can.h>
#include <lld_px4flow.h>

float gazleGetEngineSpeed ( void );
int8_t gazleGetDriverIsDemandEnginePercentTorque( void );
int8_t gazleGetActualEnginePercentTorque ( void );
double gazleGetSpeed( void );
double gazleGetSpeed_px4flow( void );
uint8_t gazleGetBrakeSwitch( void );
uint8_t gazleGetClutchSwitch( void );
float gazleGetAcceleratorPedalPosition( void );
int8_t gazleGetPercentLoadAtCurrentSpeed( void );
double gazleGetEngineFuelRate( void );
double gazleGetEngineInstantaneousFuelEconomy( void );
double gazleGetEngineThrottleValve( void );
double gazleGetBatteryPotential( void );
double gazleGetBrakePedalPosition( void );

#endif // INCLUDE_FEEDBACK_

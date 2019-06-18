#ifndef INCLUDE_FEEDBACK_
#define INCLUDE_FEEDBACK_

#include <common.h>
#include <lld_can.h>
#include <lld_px4flow.h>

float gazelGetEngineSpeed (void);
int8_t gazelGetDriverIsDemandEnginePercentTorque(void);
int8_t gazelGetActualEnginePercentTorque (void);
double gazelGetSpeed(void);
double gazelGetSpeed_px4flow(void);
uint8_t gazelGetBrakeSwitch(void);
uint8_t gazelGetClutchSwitch(void);
float gazelGetAcceleratorPedalPosition(void);
int8_t gazelGetPercentLoadAtCurrentSpeed(void);
double gazelGetEngineFuelRate(void);
double gazelGetEngineInstantaneousFuelEconomy(void);
double gazelGetEngineThrottleValve(void);
double gazelGetBatteryPotential(void);
double gazelGetBrakePedalPosition(void);

#endif // INCLUDE_FEEDBACK_
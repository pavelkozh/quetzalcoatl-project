#ifndef INCLUDE_FEEDBACK_
#define INCLUDE_FEEDBACK_

#include <common.h>
#include <lld_can.h>
#include <lld_px4flow.h>

float GazleGetEngineSpeed ();
int8_t GazleGetDriverIsDemandEnginePercentTorque();
int8_t GazleGetActualEnginePercentTorque ();
double GazleGetSpeed();
double GazleGetSpeed_px4flow();
uint8_t GazleGetBrakeSwitch();
uint8_t GazleGetClutchSwitch();
float GazleGetAcceleratorPedalPosition();
int8_t GazleGetPercentLoadAtCurrentSpeed();
double GazleGetEngineFuelRate();
double GazleGetEngineInstantaneousFuelEconomy();
double GazleGetEngineThrottleValve();
double GazleGetBatteryPotential();
double GazleGetBrakePedalPosition();

#endif // INCLUDE_FEEDBACK_
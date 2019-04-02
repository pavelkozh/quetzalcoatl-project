#ifndef INCLUDE_LLD_CAN_H_
#define INCLUDE_LLD_CAN_H_

#include <common.h>

 /*
	 PGN     PGNhex  SPN Description            		spn     Bytes		convert
		
	61444   0xF004  Engine speed                		190     4-5     	0.125rpm/bit
	61444   0xF004	Driver's Demand Engine -			512		2			1 %/bit , -125% offset
	 	 	 	 	 	 	 	 	 Percent Torque
	61444   0xF004	Actual Engine - Percent Torque		513		3			1 %/bit , -125% offset
	65265	0xFEF1	Wheel-Based Vehicle Speed 			84		2-3			1/256 km/h per bit
	65265	0xFEF1	Brake Switch						597		4.5			00 - Brake pedal released  	01 - Brake pedal depressed
	65265	0xFEF1 	Clutch Switch						598		4.7			00 - Clutch pedal released  01 - Clutch pedal depressed
	61443   0xF003  Accelerator Pedal Position  		91      2       	0.4 %/bit
	61443   0xF003  Percent Load At Current Speed		92		3			1 %/bit
	65266   0xFEF2  Engine Fuel Rate            		183     1-2     	0.05 L/h per bit
					Engine Instantaneous Fuel Economy 	184     3-4     	1/512 km/kg per bit
					Engine Throttle Valve 1 Position	51      7       	0.4 %/bit
	65271   0XFEF7  Battery Potential (V)	    		168     5-6     	0.05 V/bit
	61441 	EBC1	Brake Pedal Position 				521		2			0.4%/bit		
*/		

#define PGN_ELECTRONIC_ENGINE_CONTROLLER_1  0x00F00400
#define PGN_CRUISE_CONTROL_AND_VEHICL_SPEED 0x00FEF100
#define PGN_ELECTRONIC_ENGINE_CONTROLLER_2 	0x00F00300
#define PGN_DOOR_CONTROL					0x00FE4E00
#define PGN_FUEL_ECONOMY					0x00FEF200
#define PGN_VEHICLE_ELECTRICAL_POWER		0X00FEF700
#define PGN_ELECTRONIC_BRAKE_CONTROLLER		0x00EBC100


void can_init ( void );
static CANRxFrame rxmsg;
static CANTxFrame txmsg;

typedef struct 
{
	double EngineSpeed ;
	int8_t DriverIsDemandEnginePercentTorque;
	int8_t ActualEnginePercentTorque ;
	double Speed;
	uint8_t BrakeSwitch;
	uint8_t ClutchSwitch;
	double AcceleratorPedalPosition;
	int8_t PercentLoadAtCurrentSpeed;
	double EngineFuelRate;
	double EngineInstantaneousFuelEconomy;
	double EngineThrottleValve;
	double BatteryPotential;
	double BrakePedalPosition;
} gazelParam;


#endif /* INCLUDE_LLD_CAN_H_ */

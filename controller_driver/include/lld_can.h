#ifndef INCLUDE_LLD_CAN_H_
#define INCLUDE_LLD_CAN_H_

#include <common.h>

 /*
	 PGN     PGNhex  SPN Description            		spn     Bytes		convert
		
	61444   0xF004  Engine speed                		190     4-5     	0.125rpm/bit
	65265	0xFEF1	Wheel-Based Vehicle Speed 			84		2-3			1/256 km/h per bit
	61443   0xF003  Accelerator Pedal Position  		91      2       	0.4 %/bit
	65262   0xFEEE  Engine Oil Temperature 1    		175     3-4     	0.03125 deg C/bit 
					Engine Fuel Temperature 1   		174     2       	1 deg C/bi
	65102   0xFE4E  Position of doors           		1821    1.1     	0000 At least 1 door is open
																			0001 Closing last door
																			0010 All doors closed
																			0011-1101 Not defined
																			1110 Error
																			1111 Not available
	65266   0xFEF2  Engine Fuel Rate            		183     1-2     	0.05 L/h per bit
					Engine Instantaneous Fuel Economy 	184     3-4     	1/512 km/kg per bit
					Engine Throttle Valve 1 Position	51      7       	0.4 %/bit
	65271   0XFEF7  Net Battery Current         		114     1       	1-125 A/bit
					Alternator Current          		115     2       	1 A/bit
					Alternator Potential (V)    		167     3-4     	0.05 V/bit
					Electrical Potential (V)    		168     5-6     	0.05 V/bit
					Battery Potential (V)       		158     7-8     	0.05 V/bit
	61441 	EBC1	Brake Pedal Position 				521		2			0.4%/bit		
*/		

#define PGN_ELECTRONIC_ENGINE_CONTROLLER_1  0x00F00400
#define PGN_CRUISE_CONTROL_AND_VEHICL_SPEED 0x00FEF100
#define PGN_ELECTRONIC_ENGINE_CONTROLLER_2 	0x00F00300
#define PGN_ENGINE_TEMPERATURE_1			0x00FEEE00
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
	double Speed;
	double AcceleratorPedalPosition;
	double EngineOilTemperature;
	int16_t EngineFuelTemperature;
	int16_t Positionofdoors;
	double EngineFuelRate;
	double EngineInstantaneousFuelEconomy;
	double EngineThrottleValve;
	int16_t NetBatteryCurrent;
	int16_t AlternatorCurrent;
	double AlternatorPotential;
	double ElectricalPotential;
	double BatteryPotential;
	double BrakePedalPosition;
} gazelParam;


#endif /* INCLUDE_LLD_CAN_H_ */

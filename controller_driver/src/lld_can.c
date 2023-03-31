#include <lld_can.h>


static const CANConfig cancfg= {
CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
 CAN_BTR_SJW(0) | CAN_BTR_TS2(0) |
CAN_BTR_TS1(5) | CAN_BTR_BRP(26)
};


static CANRxFrame rxmsg;

/*0b100 - Data with EID or (0b110 - RemoteFrame with EID)*/
#define set_can_eid_data(x) ((x << 3)|0b100)

/*0b110 - Mask enable for EID/SID and DATA/RTR*/
#define set_can_eid_mask(x) ((x << 3)|0b110)


static gazelParam gazel = {
  .EngineSpeed = 0.0 ,
  .DriverIsDemandEnginePercentTorque = 0,
  .EngineDemandTorque = 0,
  .ActualEnginePercentTorque  = 0.0,
  .Speed = 0.0,
  .Speed_px4flow = 0.0,
  .AcceleratorPedalPosition = 0,
  .PercentLoadAtCurrentSpeed = 0,
  .EngineFuelRate = 0 ,
  .EngineInstantaneousFuelEconomy = 0 ,
  .EngineThrottleValve = 0 ,
  .BatteryPotential = 0 ,
  .BrakePedalPosition=0
};





void canUpdate(void){
      if (chEvtWaitAnyTimeout(ALL_EVENTS, MS2ST(100)) == 0)
        return;
      while ( canReceive(&CAND1, CAN_ANY_MAILBOX, &rxmsg, TIME_IMMEDIATE) == MSG_OK)
            {
              can_handler(rxmsg);
            }
};

//
// /*
//  * Transmitter thread.
//  */
// static THD_WORKING_AREA(can_tx_wa, 256);
// static THD_FUNCTION(can_tx, p) {
//   (void)p;
//   chRegSetThreadName("transmitter");
//   // txmsg.IDE = CAN_IDE_EXT;
//   // txmsg.EID = 0x01234567;
//   // txmsg.RTR = CAN_RTR_DATA;
//   // txmsg.DLC = 8;

//   while (true) {
//   // txmsg.data32[0] = flow_comp_m_x();
//   // txmsg.data32[1] = 0x00FF00FF;
//   //   if( canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, MS2ST(100)) == MSG_OK){
//   //        palTogglePad(GPIOB,7);
//   //   }else{
//   //        palToggleLine(LINE_LED3); 
//   //   }
//       //px4_filter();

//     chThdSleepMilliseconds(10);
//   }
// }



void can_init ( void )
{
  //Setting pin mode
    palSetPadMode(GPIOD,1,PAL_MODE_ALTERNATE(9));
    palSetPadMode(GPIOD,0,PAL_MODE_ALTERNATE(9));

  // Setting can filters
    CANFilter can_filter[8] = {\
    {0, 1, 1, 0, set_can_eid_data(0x00008000), set_can_eid_data(0x00005555)},\
    {1, 0, 1, 0, set_can_eid_data(PGN_ELECTRONIC_ENGINE_CONTROLLER_1), set_can_eid_mask(0x00ffff00)},\
    {2, 0, 1, 0, set_can_eid_data(PGN_CRUISE_CONTROL_AND_VEHICL_SPEED), set_can_eid_mask(0x00ffff00)},\
    {3, 0, 1, 0, set_can_eid_data(PGN_ELECTRONIC_ENGINE_CONTROLLER_2), set_can_eid_mask(0x00ffff00)},\
    {5, 0, 1, 0, set_can_eid_data(PGN_DOOR_CONTROL), set_can_eid_mask(0x00ffff00)},\
    {6, 0, 1, 0, set_can_eid_data(PGN_FUEL_ECONOMY), set_can_eid_mask(0x00ffff00)},\
    {7, 0, 1, 0, set_can_eid_data(PGN_VEHICLE_ELECTRICAL_POWER), set_can_eid_mask(0x00ffff00)}\
    };

    //Filters for can2 and can1 are configured together. See Reference manual( CAN filter master register (CAN_FMR) )
    canSTM32SetFilters(&CAND1,8,8,&can_filter[0]);

    //start Can
    canStart(&CAND1, &cancfg);
    //chThdCreateStatic(can_tx_wa, sizeof(can_tx_wa), NORMALPRIO + 7, can_tx, NULL);


}


void can_handler(CANRxFrame msg){
  switch((msg.EID & 0x00ffff00)){
    case PGN_ELECTRONIC_ENGINE_CONTROLLER_1: 
      gazel.EngineSpeed =0.125*( (msg.data8[4]<<8)|msg.data8[3]);
      gazel.DriverIsDemandEnginePercentTorque = msg.data8[1]-125;
      gazel.ActualEnginePercentTorque = ((float)(msg.data8[2]-125)) + (float)(msg.data8[0] >> 4)*0.125;
      gazel.EngineDemandTorque = msg.data8[7]-125;
      break;
    case PGN_CRUISE_CONTROL_AND_VEHICL_SPEED:
      gazel.Speed = ((msg.data8[2]<<8)|msg.data8[1])/256.0;

      gazel.BrakeSwitch = (msg.data8[3]>>4) & 0x03;
      gazel.ClutchSwitch = (msg.data8[3]>>6) & 0x03;

      break;
    case PGN_ELECTRONIC_ENGINE_CONTROLLER_2:
      gazel.AcceleratorPedalPosition = 0.4*msg.data8[1];
      gazel.PercentLoadAtCurrentSpeed = msg.data8[3];
      break;
    case PGN_FUEL_ECONOMY:
      gazel.EngineFuelRate = 0.05* ( (msg.data8[1]<<8)|msg.data8[0]);
      gazel.EngineInstantaneousFuelEconomy = ((msg.data8[3]<<8)|msg.data8[2])/512;
      gazel.EngineThrottleValve = 0.4*msg.data8[6];
      break;
    case PGN_VEHICLE_ELECTRICAL_POWER:
      gazel.BatteryPotential = 0.05* (double)( (msg.data8[5]<<8) | msg.data8[6]);
    case PGN_ELECTRONIC_BRAKE_CONTROLLER:
      gazel.BrakePedalPosition = msg.data8[1]*0.4;
      break;
  };

}


gazelParam* gazelGetStruct(void){
  return &gazel;
};



// void canEngineSpeed(void){
//   return gazel.EngineSpeed;
// }
// void canDriverIsDemandEnginePercentTorque(void){
//   return gazel.DriverIsDemandEnginePercentTorque;
// }
// void canActualEnginePercentTorque(void){
//   return gazel.ActualEnginePercentTorque;
// }
// void canSpeed(void){
//   return gazel.Speed;
// }
// void canSpeed_px4flow(void){
//   return gazel.Speed_px4flow;
// }
// void canAcceleratorPedalPosition(void){
//   return gazel.AcceleratorPedalPosition;
// }
// void canPercentLoadAtCurrentSpeed(void){
//   return gazel.PercentLoadAtCurrentSpeed;
// }
// void canEngineFuelRate(void){
//   return gazel.EngineFuelRate;
// }
// void canEngineInstantaneousFuelEconomy(void){
//   return gazel.EngineInstantaneousFuelEconomy;
// }
// void canEngineThrottleValve(void){
//   return gazel.EngineThrottleValve;
// }
// void canBatteryPotential(void){
//   return gazel.BatteryPotential;
// }
// void canBrakePedalPosition(void){
//   return gazel.BrakePedalPosition;
// }

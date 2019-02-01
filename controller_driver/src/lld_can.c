#include <lld_can.h>



static const CANConfig cancfg= {
CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
 CAN_BTR_SJW(0) | CAN_BTR_TS2(0) |
CAN_BTR_TS1(5) | CAN_BTR_BRP(26)
};



/*
 * Receiver thread.
 */
static THD_WORKING_AREA(can_rx_wa, 256);
static THD_FUNCTION(can_rx, arg) {
    arg = arg;

    event_listener_t el2;
    event_listener_t el1;

    chRegSetThreadName("receiver");
    chEvtRegister(&CAND2.rxfull_event, &el2, 0);
    chEvtRegister(&CAND1.rxfull_event, &el1, 0);

    while (1)
    {
      if (chEvtWaitAnyTimeout(ALL_EVENTS, MS2ST(100)) == 0)
        continue;
      while ( canReceive(&CAND2, CAN_ANY_MAILBOX, &rxmsg, TIME_IMMEDIATE) == MSG_OK)
      {
        /* Process message.*/
        palTogglePad(GPIOB,0); //Зелёный
      }
      while ( canReceive(&CAND1, CAN_ANY_MAILBOX, &rxmsg, TIME_IMMEDIATE) == MSG_OK)
            {
              /* Process message.*/
              palTogglePad(GPIOB,14); //Красный
              txmsg.data32[0] = rxmsg.data32[0];
              txmsg.data32[1] = rxmsg.data32[1];
            }
      chThdSleepMilliseconds( 10 );

    }
    chEvtUnregister(&CAND2.rxfull_event, &el2);
    chEvtUnregister(&CAND1.rxfull_event, &el1);

  }

/*
 * Transmitter thread.
 */
static THD_WORKING_AREA(can_tx_wa, 256);
static THD_FUNCTION(can_tx, p) {
  (void)p;
  chRegSetThreadName("transmitter");
  txmsg.IDE = CAN_IDE_EXT;
  txmsg.EID = 0x01234567;
  txmsg.RTR = CAN_RTR_DATA;
  txmsg.DLC = 8;
  txmsg.data32[0] = 0x55AA55AA;
  txmsg.data32[1] = 0x00FF00FF;

  while (true) {

    if( canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, MS2ST(100)) == MSG_OK){
         palTogglePad(GPIOB,7);//Синий
    }else{
//         palTogglePad(GPIOB,14); //Крачный
    }

    chThdSleepMilliseconds(500);
  }
}


/*0b100 - Data with EID or (0b110 - RemoteFrame with EID)*/
#define set_can_eid_data(x) ((x << 3)|0b100)

/*0b110 - Mask enable for EID/SID and DATA/RTR*/
#define set_can_eid_mask(x) ((x << 3)|0b110)

void can_init ( void )
{
  //Setting pin mode
    palSetPadMode(GPIOD,1,PAL_MODE_ALTERNATE(9));
    palSetPadMode(GPIOD,0,PAL_MODE_ALTERNATE(9));
    palSetPadMode(GPIOB,5,PAL_MODE_ALTERNATE(9));
    palSetPadMode(GPIOB,6,PAL_MODE_ALTERNATE(9));
  // Setting can filters
    CANFilter can_filter[2] = {\
    {0, 1, 1, 0, set_can_eid_data(0x00008000), set_can_eid_data(0x00005555)},\
    {1, 0, 1, 0, set_can_eid_data(0x00005555), set_can_eid_mask(0x0000fff0)}\
    };

    //Filters for can2 and can1 are configured together. See Reference manual( CAN filter master register (CAN_FMR) )
    canSTM32SetFilters(&CAND1,1,2,&can_filter[0]);

    //start Can
    canStart(&CAND1, &cancfg);
    canStart(&CAND2, &cancfg);
    chThdCreateStatic(can_rx_wa, sizeof(can_rx_wa), NORMALPRIO + 7, can_rx, NULL);
    chThdCreateStatic(can_tx_wa, sizeof(can_tx_wa), NORMALPRIO + 6, can_tx, NULL);

}








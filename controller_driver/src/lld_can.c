#include <lld_can.h>



static const CANConfig cancfg1 = {
CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
 CAN_BTR_SJW(0) | CAN_BTR_TS2(0) |
CAN_BTR_TS1(5) | CAN_BTR_BRP(26)
};
static const CANConfig cancfg2 = {
CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
 CAN_BTR_SJW(0) | CAN_BTR_TS2(0) |
CAN_BTR_TS1(5) | CAN_BTR_BRP(26)
};


/*
 * Receiver thread.
 */
static THD_WORKING_AREA(can_rx1_wa, 256);
static THD_WORKING_AREA(can_rx2_wa, 256);
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
  CANTxFrame txmsg;
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

void can_init ( void )
{
    palSetPadMode(GPIOD,1,PAL_MODE_ALTERNATE(9));
    palSetPadMode(GPIOD,0,PAL_MODE_ALTERNATE(9));
    palSetPadMode(GPIOB,5,PAL_MODE_ALTERNATE(9));
    palSetPadMode(GPIOB,6,PAL_MODE_ALTERNATE(9));
    canStart(&CAND1, &cancfg1);
    canStart(&CAND2, &cancfg2);
    chThdCreateStatic(can_rx1_wa, sizeof(can_rx1_wa), NORMALPRIO + 7, can_rx, NULL);
    chThdCreateStatic(can_rx2_wa, sizeof(can_rx2_wa), NORMALPRIO + 7, can_rx, NULL);
    chThdCreateStatic(can_tx_wa, sizeof(can_tx_wa), NORMALPRIO + 6, can_tx, NULL);

}








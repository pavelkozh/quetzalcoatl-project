#include <lld_can.h>



static const CANConfig cancfg = {
CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
CAN_BTR_SJW(0) | CAN_BTR_TS2(0) |
CAN_BTR_TS1(5) | CAN_BTR_BRP(26)
};


/*
 * Receiver thread.
 */
static THD_WORKING_AREA(can_rx1_wa, 256);
static THD_FUNCTION(can_rx, arg) {
  arg = arg;

  while (1)
  {
    if (chEvtWaitAnyTimeout(ALL_EVENTS, MS2ST(100)) == 0)
      continue;
    while ( canReceive(&CAND1, CAN_ANY_MAILBOX, &rxmsg, TIME_IMMEDIATE) == MSG_OK)
    {
      /* Process message.*/
      palTogglePad( GPIOA, 0 );
    }
    chThdSleepMilliseconds( 10 );
  }
}

void can_init ( void )
{
    canStart(&CAND1, &cancfg);
    chThdCreateStatic(can_rx1_wa, sizeof(can_rx1_wa), NORMALPRIO + 7, can_rx, NULL);

}








#include <lld_can.h>

#define CAN_MAILBOX_BUFFER_SIZE 20
#define CAN_TIME_STAMP_MS 100


static bool         isInitialized           = false;


#if 0

static void rxfull_cb(CANDriver *canp, uint32_t flags) {}
static void txempty_cb(CANDriver *canp, uint32_t flags) {}
static void error_cb(CANDriver *canp, uint32_t flags) {}

mailbox_t can_rx_mb;
msg_t can_mb_buffer[CAN_MAILBOX_BUFFER_SIZE];
MAILBOX_DECL(can_rx_mb, can_mb_buffer, CAN_MAILBOX_BUFFER_SIZE);

static const CANRxFrame can_rx_frame_conf = {
  /*Filter match identifier.
   * This register contains the index of the filter the message stored in
   * the mailbox passed through. For beginning try 0.
   */
  .FMI = 0,

  /*Time stamp.
   *This field contains the 16-bit timer value captured at the SOF transmission.
   */
  .TIME =  MS2ST(CAN_TIME_STAMP_MS),

  /*Data length code
   *This field defines the number of data bytes a data frame contains or a remote frame request.
   *A message can contain from 0 to 8 data bytes, depending on the value in the DLC field.
   */
  .DLC = 8,

  /*Remote transmission request
   * 0 - Data frame
   * 1 - Remote frame
   */
  .RTR = 0,

  /*Identifier type
   * 0 - standard
   * 1 -extended
   */
  .IDE = 1

};
#endif

#if 1
/* CAN instance configuration */
static CANConfig cancfg = {
  CAN_MCR_ABOM | CAN_MCR_AWUM,
  CAN_BTR_SJW(0) | CAN_BTR_TS2(1) |
  CAN_BTR_TS1(8) | CAN_BTR_BRP(6)
};
#endif

CANRxFrame rxmsg;
int iFilterMask = 0;
int iFilterValue = 0;

#if 0
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
    canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, MS2ST(100));
    chThdSleepMilliseconds(500);
  }
}
#endif

void canNetInit ( void )
{
  if ( isInitialized )
      return;

  //canInit(); //???
  canStart(&CAND1,NULL);

  isInitialized = true;
}

uint8_t rxmsg_data[8];

void canRead ( void )
{

  //if ( !isInitialized )
  //    return -1;

  while (canReceive (&CAND1, CAN_ANY_MAILBOX, &rxmsg, TIME_IMMEDIATE) == MSG_OK)
  {
      if ((rxmsg.EID & iFilterMask) != (iFilterValue & iFilterMask))
      {
        //rxmsg_data = rxmsg.data8;
      }
  }


}



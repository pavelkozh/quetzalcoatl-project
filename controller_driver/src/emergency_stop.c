#include <emergency_stop.h>
#include <pedals.h>

#define EMERGENCY_STOP_LINE PAL_LINE( GPIOA, 0 )

static thread_reference_t trp_emergency_button_stop = NULL;
// controls thread's state: when false, thread sleeps
static bool is_braking_thread_work = false;
/* flag is true when button for emergency stop control was pressed
   (after bounce of contacts is filtered) */
static bool is_emergency_stop_btn_pressed = false;

static THD_WORKING_AREA(emergency_button_stop_wa, 256);

/*
 * @brief function for emergency stop thread
 * @note when is_braking_thread_work is false, the emergency stop thread sleeps
 *       when is_braking_thread_work is true, wake up the thread, press clutch -> press brake -> turn gear to neutrall -> switch off
 */
static THD_FUNCTION(emergency_button_stop, arg) {
    (void)arg;
    while(1){
        if ( is_braking_thread_work == false )
        {
            chSysLock();
            chThdSuspendS(&trp_emergency_button_stop);
            chSysUnlock();
        }
        else
        {
            pedalsClutchPress ( 1000 );
            if ( pedalsClutchGetState () == false )
            {
              pedalsBrakePress( 5000 );
              if ( pedalsBrakeGetState() == false )
              {
                  // gear = neutral!
                  // zagiganie off!!!
                  is_braking_thread_work = false;
                  chSysLock();
                  chThdSuspendS(&trp_emergency_button_stop);
                  chSysUnlock();
              }
            }
        }

        chThdSleepMilliseconds( 1 );
    }
}

/*
 * @brief interrupt handling function
 * @note turn is_braking_thread_work to true, then wake up the thread for stop process
 */
void extcb_base(EXTDriver *extp, expchannel_t channel)
{
    /* To avoid warnings*/
    (void)extp;
    (void)channel;

    /* Set flag */
    //palToggleLine(LINE_LED1);
    is_braking_thread_work = true;

    /* Wake up braking thread */
    chSysLock();
    chThdResume(&trp_emergency_button_stop, MSG_OK);
    chSysUnlock();


}

static bool isInitialized = false;

/*
 * @brief   Initialize pedals module, interrupt for emergency stop button and create thread for stop process
 */
void emergencyStopInit( void ) {

	if ( isInitialized )
		return;
    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );   //Led

    pedalsInit();
    chThdCreateStatic(emergency_button_stop_wa, sizeof(emergency_button_stop_wa), NORMALPRIO + 15, emergency_button_stop, NULL);
	palSetLineMode( PAL_LINE(GPIOA,0), PAL_MODE_INPUT_PULLUP);
	commonExtDriverInit();
	EXTChannelConfig base_conf = {
	    .mode = EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOA, 
	    .cb = extcb_base
	};

	extSetChannelMode( &EXTD1, 0, &base_conf );



	isInitialized = true;
}

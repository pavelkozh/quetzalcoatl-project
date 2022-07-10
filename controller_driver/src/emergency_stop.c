#include <emergency_stop.h>
#include <MT_control.h>
#include <pedals.h>


#define EMERGENCY_STOP_LINE       PAL_LINE( GPIOA, 0 )

static thread_reference_t trp_emergency_button_stop = NULL;
// controls thread's state: when false, thread sleeps
static bool is_breaking_thread_work = false;
/* flag is true when button for emergency stop control was pressed
   (after bounce of contacts is filtered) */
static bool is_emergency_stop_btn_pressed = false;

static THD_WORKING_AREA(emergency_button_stop_wa, 256);

/*
 * @brief function for emergency stop thread
 * @note when is_breaking_thread_work is false, the emergency stop thread sleeps
 *       when is_breaking_thread_work is true, wake up the thread, press clutch -> press brake -> turn gear to neutrall -> switch off
 */

static THD_FUNCTION(emergency_button_stop, arg) {
    (void)arg;

    while(1){

        if ( is_breaking_thread_work == false ){
            chSysLock();
            chThdSuspendS(&trp_emergency_button_stop);
            chSysUnlock();
        }
        else{
           // soundSignalStartContiniousSignals ();
            //palSetLine(LINE_LED2);

            pedalsBrakePress( 1000 );
            while(pedalsBrakeGetState()){
                chThdSleepMilliseconds( 50 );
            }

            pedalsClutchPress ( 1000 );
            while(pedalsClutchGetState()){
                chThdSleepMilliseconds( 50 );
            }

            while ( mtControlMannualyShiftGear(0) !=0 ){
                chThdSleepMilliseconds( 50 );
            }


//                  pedalsClutchRelease(1000);
                  

//                if (is_emergency_stop_btn_pressed == true) {
////                  chprintf( (BaseSequentialStream *)&SD3, "Switch off\n\r");
//                  // zagiganie off!!!
//                }
             is_breaking_thread_work = false;
             is_emergency_stop_btn_pressed = false;
           //  palClearLine(LINE_LED2);
             /*chSysLock();
             chThdSuspendS(&trp_emergency_button_stop);
             chSysUnlock();*/
//              }
//            }
        }
      //}
      chThdSleepMilliseconds( 50 );
      
    }
}
/*
 * @brief   Executes full stop WITHOUT swiching off. Can de called from any module
 */
bool emergencyFullStop (void) {
  if (is_breaking_thread_work == false) {
      is_breaking_thread_work = true;
      /* Wake up braking thread */
      chSysLock();
      chThdResumeS(&trp_emergency_button_stop, MSG_OK);
      chSysUnlock();
  }
  return (is_breaking_thread_work && (pedalsClutchGetPosition() == pedalsClutchGetMaxPosition()) && (pedalsBrakeGetPosition() == pedalsBrakeGetMaxPosition()) && (mtControlGetCurrentGearNum()==0));
}

/*
 * @brief interrupt handling function
 * @note turn is_breaking_thread_work to true, then wake up the thread for stop process
 */
void extcb_base(EXTDriver *extp, expchannel_t channel)
{
    /* To avoid warnings*/
    (void)extp;
    (void)channel;
    /* Set flag */
    //palToggleLine(LINE_LED1);

    //if (palReadPad(GPIOA,0)==0){


/*
        if (is_breaking_thread_work == false) {
            is_emergency_stop_btn_pressed = true;
            is_breaking_thread_work = true;
    //        chprintf( (BaseSequentialStream *)&SD3, "In Interrupt\n\r");

            chSysLockFromISR();
            chThdResumeI(&trp_emergency_button_stop, MSG_OK);
            chSysUnlockFromISR();
        }

*/
        palToggleLine(LINE_LED3);

        //}

}

static bool isInitialized = false;

/*
 * @brief   Initialize pedals module, interrupt for emergency stop button and create thread for stop process
 */
void emergencyStopInit( void ) {

	if ( isInitialized )
		return;
//    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led
//    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led
//    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );   //Led

    //pedalsInit();
    //mtControlInit();
    chThdCreateStatic(emergency_button_stop_wa, sizeof(emergency_button_stop_wa), NORMALPRIO + 15, emergency_button_stop, NULL);
  	//palSetLineMode( PAL_LINE(GPIOA,0), PAL_MODE_INPUT_PULLDOWN);
  	//commonExtDriverInit();
  	//EXTChannelConfig base_conf = {
  	/*    .mode = EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOA,
  	    .cb = extcb_base
  	};*/

	//extSetChannelMode( &EXTD1, 0, &base_conf );



	isInitialized = true;
}


#include <tests.h>
#include <chprintf.h>

#include <common.h>



static thread_reference_t trp_start = NULL;
static bool start_enable_thr = false;

static THD_WORKING_AREA(start_wa, 512);
static THD_FUNCTION(start_thr, arg) {
    (void)arg;

    while(1){
        if ( start_enable_thr == false ){
            chSysLock();
            chThdSuspendS(&trp_start);
            chSysUnlock();
        }
        else{

            pedalsBrakeRelease( 1000 );
            comm_dbgprintf_info("brake releaase");
            while(pedalsBrakeGetPosition() != 0){
               chThdSleepMilliseconds( 50 );
            }

           pedalsClutchPress( 1000 );
           comm_dbgprintf_info(" Clutch Press ");
           while(pedalsClutchGetPosition() <= 95000){
               chThdSleepMilliseconds( 50 );
           }

           mtControlMannualyShiftGear(1);
           while ( mtControlGetCurrentGearNum() != 1 ){
               comm_dbgprintf_info(" Gear = 1 ");
               chThdSleepMilliseconds( 50 );
           }


           pedalsClutchRelease ( 10000 );
           while(pedalsClutchGetPosition() > 1000){
              chThdSleepMilliseconds( 50 );
          }

           start_enable_thr = false;


        }



        chThdSleepMilliseconds( 50 );

    }
}



void commandSet( int8_t avto_speed, int8_t steer_speed ) {
    //speed_ref = avto_speed;
    comm_dbgprintf("Avto speed reference:      %d\t \n\r", avto_speed);

    if ((avto_speed < 2) && (avto_speed > -2))
    {
        emergencyFullStop();
    }
    else{
        if (start_enable_thr == false) {
            start_enable_thr = true;
              /* Wake up braking thread */
              chSysLock();
              chThdResumeS(&trp_start, MSG_OK);
              chSysUnlock();
        }
    }

    //TODO: ADD Steer control
    if ( steer_speed > 0 ){
        if ( steer_speed >= 100){
            steer_speed = 100;
        }
        else if ( steer_speed < 5)
        {
            steer_speed = 0;
        }
    }
    if ( steer_speed < 0 ){
        palSetLine(LINE_LED1);
        if ( steer_speed <= -100){
            steer_speed = -100;
        }
        else if ( steer_speed > -5)
        {
            steer_speed = 0;
        }
    }

//    steer_speed_ref = (uint16_t)( 1000000.0/ ( 0.842 * (float)steer_speed + 15.79) );
//    comm_dbgprintf("steer speed reference:      %d\t \n\r", steer_speed_ref);

    if ( (steer_speed >= 5 ) &&  (steer_speed <= 100 ) )
    {
        lldSteerSMMoveToTheRight((uint16_t)( 1000000.0/ ( 0.842 * (float)steer_speed + 15.79) ));
    }
    else if ( (steer_speed >= -100 ) &&  (steer_speed <= -5 ) )
    {
        lldSteerSMMoveToTheLeft((uint16_t)( 1000000.0/ ( 0.842 * (float)steer_speed + 15.79) ));
    }
    else /* case steer_speed = 0 */ {
        lldSteerSMStop();
    }
}


void commandSteerStraight(void){

//    lldSteerSMSetPosition(0, 20000);
//    pedalsBrakeRelease(2000);
//    pedalsClutchRelease(2000);

    emergencyFullStop();

    comm_dbgprintf_info("STOP");

    comm_dbgprintf("*************************************************************************\n\r");
    comm_dbgprintf("                                                                         \n\r");
    comm_dbgprintf("                                                                         \n\r");
    comm_dbgprintf("                                                                         \n\r");
    comm_dbgprintf("*************************************************************************\n\r");
}

void commandStop(void){
    //TODO: Command to stop
//    engIgnitionSwitchOff();
//    soundSignalStopContiniousSignals();
    pedalsBrakeRelease(2000);
    comm_dbgprintf("STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP STOP");
}

void commandStart(void){
    //TODO: Command to start
    engIgnitionSwitchOn ();
    engStarterSwitchOn ();
   // soundSignalStartContiniousSignals();
    comm_dbgprintf("START START START START START START START START START START START ");
}

void connectionErrorCb ( void ){
    comm_dbgprintf_error("Connection fail");
    emergencyFullStop();
}






/* after this time connection error callback is execute*/
#define CONNECTION_FAIL_OK_DELAY 3000  //ms

void testMain( void ){

    feedbackInit();
    pedalsInit();
    mtControlInit();
    emergencyStopInit();
    engIgnitionInit();
    lldSteerSMInit();
    soundSignalInit();

    chThdCreateStatic(start_wa, sizeof(start_wa), NORMALPRIO, start_thr, NULL);

    communicationEventFun_t structForFunc = getDefaultCfg();

    structForFunc.on_set             = commandSet;
    structForFunc.on_start           = commandStart;
    structForFunc.on_stop            = commandStop;
    structForFunc.on_steer_straight  = commandSteerStraight;
    structForFunc.on_interrupt_timer = connectionErrorCb;

    comm_init(&structForFunc, CONNECTION_FAIL_OK_DELAY, true);
    comm_start_protocol();

    BaseChannel *dbg_chn = comm_get_channel();





    char  sd_buff[10] = {'?','?','?','?','?','?','?','?','?','?'} ;
    bool start_flag = 0;
    int8_t state = 0;

    while(1){

        palToggleLine(LINE_LED2);

        sdReadTimeout( &SD3, sd_buff, 10, TIME_IMMEDIATE   );

        if(sd_buff[0] == 's')  commandSet( atoi(&sd_buff[1]), 0);
       // if(sd_buff[0] == 'p')  main_state = MAIN_STATE_STOP;

        for (int i = 0; i < 9; i++)
        {
        sd_buff[i]='?';
        }

        //chprintf( (BaseSequentialStream *)&SD3, "gear_num: %d\t Clutch_pos: %d\t Break_pos: %d\t speed %.02f\t eng_speed %.02f\t \n\r",mtControlGetCurrentGearNum(), pedalsClutchGetPosition(),pedalsBrakeGetPosition(), gazelGetSpeed(), gazelGetEngineSpeed());
//        comm_dbgprintf("Main State: %d\t gear_num: %d\t Clutch_pos: %d\t ",main_state, mtControlGetCurrentGearNum(), pedalsClutchGetPosition());
//        comm_dbgprintf("Break_pos: %d\t speed %.02f\t eng_speed %.02f\t \n\r",pedalsBrakeGetPosition(), gazelGetSpeed(), gazelGetEngineSpeed());
//        if(gazelGetI2cErrFlag())
//            comm_dbgprintf("I2c error!\r\n");
        chThdSleepMilliseconds(500);
    }
}


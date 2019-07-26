#include <tests.h>
#include <chprintf.h>

#include <common.h>

static int8_t mt_shifting = -1; // -1-> no shifting

//static thread_reference_t trh_mt_shift = NULL;

static THD_WORKING_AREA(mt_shift_wa, 512);
static THD_FUNCTION(mt_shift, arg) {
    (void)arg;

    while(1){
        if ( mt_shifting != -1 )
        {
            if(pedalsClutchGetPosition() != pedalsClutchGetMaxPosition())
            {
                pedalsClutchPress(1000);
            }
            if (pedalsClutchGetPosition() == pedalsClutchGetMaxPosition())
            {
                switch (mt_shifting){
                    case 0: mtControlMannualyShiftGear(0);
                            break;

                    case 1: mtControlMannualyShiftGear(1);
                            break;

                    case 6: mtControlMannualyShiftGear(6);
                            break;
                }
                if (mtControlGetCurrentGearNum() == mt_shifting )
                {
                    mt_shifting = -1;
                }


            }
        }


        chThdSleepMilliseconds( 100 );

    }
}

static int8_t in_set = 0;
void commandSet( int8_t vert_djoystic, int8_t steer_speed ) {
    in_set = vert_djoystic;
    if ((vert_djoystic <= 5) || (mt_shifting != -1))
    {
        pedalsClutchMove( pedalsClutchGetMaxPosition(), 1000 );
    }
    else{
        if (vert_djoystic > 5)
        {
            pedalsBrakeMove( 0,1000 );
            if ( pedalsBrakeGetPosition() == 0) //TODO: check MT!!!!
            {
                pedalsClutchMove(pedalsClutchGetMaxPosition() - vert_djoystic * 1000, 10000 );
            }
        }
    }
    if (vert_djoystic <= -5)
    {
        if ( pedalsClutchGetPosition() >= 60000)
        {
            pedalsBrakeMove(( (-vert_djoystic - 5) * 400), 1000) ;
        }

    }


    //TODO: ADD Steer control
    if ( steer_speed > 0 ){
        if ( steer_speed >= 10){
            steer_speed = 10;
        }
        else if ( steer_speed < 2)
        {
            steer_speed = 0;
        }
    }
    if ( steer_speed < 0 ){
        palSetLine(LINE_LED1);
        if ( steer_speed <= -10){
            steer_speed = -10;
        }
        else if ( steer_speed > -2)
        {
            steer_speed = 0;
        }
    }



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

    lldSteerSMSetPosition(0, 20000);
}

void commandStop(void){
    engIgnitionSwitchOff();

    speedEngineControlStop();
}

void commandStart(void){
    engStarterSwitchOn ();
    soundSignalStartContiniousSignals();
    speedSetEnginePIDReferenceValue ( 1200.0 ) ;
    speedEngineControlStart();

}

void connectionErrorCb ( void ){
    comm_dbgprintf_error("Connection fail");
    emergencyFullStop();
}

void commandGearNeutral(void){
    mt_shifting = 0;
}
void commandGearFirst(void){
    mt_shifting = 1;
}
void commandGearSecond(void){
    mt_shifting = 6;
}






/* after this time connection error callback is execute*/
#define CONNECTION_FAIL_OK_DELAY 3000  //ms

void testMainNew( void ){

    feedbackInit();
    pedalsInit();
    mtControlInit();
    emergencyStopInit();
    engIgnitionInit();
    lldSteerSMInit();
    //soundSignalInit();
    speedInit();

    chThdCreateStatic(mt_shift_wa, sizeof(mt_shift_wa), NORMALPRIO, mt_shift, NULL);

    communicationEventFun_t structForFunc = getDefaultCfg();

    structForFunc.on_set             = commandSet;
    structForFunc.on_start           = commandStart;
    structForFunc.on_stop            = commandStop;
    structForFunc.on_steer_straight  = commandSteerStraight;
    structForFunc.on_interrupt_timer = connectionErrorCb;
    structForFunc.on_gear0           = commandGearNeutral;
    structForFunc.on_gear1           = commandGearFirst;
    structForFunc.on_gear6           = commandGearSecond;

    comm_init(&structForFunc, CONNECTION_FAIL_OK_DELAY, true);
    comm_start_protocol();

    BaseChannel *dbg_chn = comm_get_channel();


    while(1){

        palToggleLine(LINE_LED2);

        if ( mt_shifting != -1)
        {
            comm_dbgprintf ("Gear shifting = %d\t",mt_shifting);
        }

        comm_dbgprintf("speed: %d\t eng_speed: %d\t gear: %d\t gear_flag: %d\t clutch pos: %d\t brake pos: %d\t  \n\r",(uint16_t)gazelGetSpeed(), (uint16_t)gazelGetEngineSpeed(), mtControlGetCurrentGearNum(), pedalsClutchGetPosition(), pedalsBrakeGetPosition() );

        chThdSleepMilliseconds(500);
    }
}

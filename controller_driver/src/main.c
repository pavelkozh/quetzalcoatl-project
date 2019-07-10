#include <common.h>
#include <tests.h>
#include <chprintf.h>
#include <speed.h>
#include <emergency_stop.h>

// Main switch state
#define MAIN_STATE_NOP              0
#define MAIN_STATE_START            1
#define MAIN_STATE_STOP             2
#define MAIN_STATE_SPEEED_CONTROL   3
#define MAIN_STATE_BACKWARD         4

#define MAIN_STATE_START_CON       101
#define MAIN_STATE_BACKWARD_CON     104


//Start funtcion parameters
#define FORWARD 1
#define BACKWARD 6 // так как в функцию mtControlMannualyShiftGear удобнее передавать 6 для включения задней передачи=))


uint8_t main_state = 0;

bool start( uint8_t dir );

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

static float speed_ref =0;

void onSet( float speed, float angle ) {
    speed_ref = speed;
    if (speed < 1.0 && speed > -1.0) {
       main_state = MAIN_STATE_STOP;
    }
    else if ( gazelGetSpeed() < 1.0 ) {
       // pedalsBrakeRelease( 1000 );
        if(speed > 0)
            main_state = MAIN_STATE_START;
        else
            main_state = MAIN_STATE_BACKWARD;
    }

    //steerSet...


}

void onStart( void ) {
    chprintf( (BaseSequentialStream *)&SD3,"In function onStart\n\r" );
}

void onStop( void ) {
    chprintf( (BaseSequentialStream *)&SD3,"In function onStop\n\r" );
}




static int8_t statrt_fun_state = 0 ;
static int16_t _speed = 0;
bool start( uint8_t dir ){
    if(mtControlGetCurrentGearNum() != dir){
        statrt_fun_state = 0;
        if(pedalsClutchGetPosition() <= 95000)
            pedalsClutchPress(650);
        else{
            mtControlMannualyShiftGear(dir);
        }
    }else{
        switch(statrt_fun_state){
            case 0: 
                if(pedalsClutchGetPosition()>75000)
                    pedalsClutchRelease(1000);
                else{
                    pedalsClutchRelease(15000);
                    pedalsBrakeRelease(1000);
                }
                if(_speed>1.5)//gazelGetSpeed()>1.5)
                    statrt_fun_state = -1;
                break;

            case 1: 
                    pedalsClutchRelease(4000);
                    return 1;
                break;

            case -1:
                    pedalsClutchRelease(20000);
                    if(_speed>4.5)//gazelGetSpeed()>4.5)
                        statrt_fun_state = 1;
                break;
            default: break;
        }
    }
    return 0;
};

int main(void)
{
    chSysInit();
    halInit();

    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX

    feedbackInit();
    pedalsInit();
    mtControlInit();
    speedInit();
    emergencyStopInit();

    char  sd_buff[10] = {'?','?','?','?','?','?','?','?','?','?'} ;
    bool start_flag = 0;
    int8_t state = 0;
    #if (MAIN_PROGRAM_ROUTINE != PROGRAM_ROUTINE_MASTER) // need fixing

        testsRoutines();

    #else




while(1){
    sdReadTimeout( &SD3, sd_buff, 10, TIME_IMMEDIATE   );

    if(sd_buff[0] == 's')  onSet( atoi(&sd_buff[1]), 0);
    if(sd_buff[0] == 'p')  main_state = MAIN_STATE_STOP;
    if(sd_buff[0] == 'z')  _speed =  atoi(&sd_buff[1]);

//
//    chprintf( (BaseSequentialStream *)&SD3,"state:\t%d\tCl_pos:\t%d\tspeed\t%.2f\tgear: %d\n\r",state,pedalsClutchGetPosition(),gazelGetSpeed(),mtControlGetCurrentGearNum());
    for (int i = 0; i < 9; i++)
    {
      sd_buff[i]='?';
    }

    switch(main_state){
    case MAIN_STATE_START:
        statrt_fun_state = 0;
        main_state = MAIN_STATE_START_CON;
        break;
    case MAIN_STATE_START_CON:
        if(start(FORWARD)){
            main_state = MAIN_STATE_SPEEED_CONTROL;
        }
        break;

    case MAIN_STATE_STOP:
        speedVehicleControlStop();
        if(emergencyFullStop()){
            main_state = MAIN_STATE_NOP;
        }
        break;


    case MAIN_STATE_SPEEED_CONTROL:
            speedVehicleControlStart();
            speedSetVehiclePIDReferenceValue(speed_ref);
        break;
    case MAIN_STATE_BACKWARD:
        statrt_fun_state = 0;
        main_state = MAIN_STATE_BACKWARD_CON;
        break;
    case MAIN_STATE_BACKWARD_CON:
        if(start(BACKWARD)){
                    main_state = MAIN_STATE_NOP;
                }
        break;
    default:
        break;
    }

    chprintf( (BaseSequentialStream *)&SD3, "Main State: %d gear_num: %d Clutch_pos: %d Break_pos: %d \n\r",main_state, mtControlGetCurrentGearNum(),pedalsClutchGetPosition(),pedalsBrakeGetPosition());

    chThdSleepMilliseconds(100);
}


    #endif

}

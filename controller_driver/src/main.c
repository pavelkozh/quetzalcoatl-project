#include <common.h>
#include <tests.h>
#include <chprintf.h>



// Main switch state
#define MAIN_STATE_NOP              0
#define MAIN_STATE_START            1
#define MAIN_STATE_STOP             2
#define MAIN_STATE_SPEEED_CONTROL   3
#define MAIN_STATE_BACKWARD         4

#define MAIN_STATE_START_CON        101
#define MAIN_STATE_BACKWARD_CON     104


//Start funtcion parameters
#define FORWARD  1
#define BACKWARD 6 // 6 gear means backward


bool start( uint8_t dir );

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

static uint8_t main_state = 0;
static float speed_ref = 0;
static uint8_t speed_sign = 1;
static uint8_t speed_ref_sign = 1;

/*
 * Speed direction. Stands for initial state
 * 1 - speed~(-5:5) km/h
 * 0 - speed>5 km/h
 * 2 - speed<-5 km/h
 */
/*
                 |-----------ref_sign-----------|
    |speed_sign__|___0_______|___1__|___2_______|
    |      0     | SpeedCont | Stop | Stop      |
    |      1     | Start (f) | NOP  | Start (b) |
    |      2     | Stop      | Stop | SpeedCont |
 */

static uint8_t arr[3][3] = {{1,2,3},{4,5,6},{7,8,9}};

void onSet( float speed, float angle ) {
    speed_ref = speed;
    if (speed < 4.0 && speed > -4.0) {
        speed_ref_sign = 1;
    }
    else if ( gazelGetSpeed() < 1.0 ) {

        if(speed > 0)
            speed_ref_sign = 0;
        else
            speed_ref_sign = 2;
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
                pedalsBrakeRelease(1000);
                if(pedalsClutchGetPosition()>75000)
                    pedalsClutchRelease(1000);
                else{
                    pedalsClutchRelease(15000);

                }
                if(gazelGetSpeed()>1.5)
                    statrt_fun_state = -1;
                break;

            case 1: 
                    pedalsClutchRelease(4000);
                    return 1;
                break;

            case -1:
                    pedalsClutchRelease(20000);
                    if(gazelGetSpeed()>4.5)
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


#if (MAIN_PROGRAM_ROUTINE != PROGRAM_ROUTINE_MASTER) // need fixing

    testsRoutines();

#else

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

        switch (arr[speed_sign][speed_ref_sign]) {
            case 1:
                main_state = MAIN_STATE_SPEEED_CONTROL;
                break;
            case 2:
                main_state = MAIN_STATE_STOP;
                break;
            case 3:
                main_state = MAIN_STATE_STOP;
                break;
            case 4:
                if( (main_state == MAIN_STATE_START) || (main_state == MAIN_STATE_START_CON))
                    main_state = MAIN_STATE_START_CON;
                else
                    main_state  = MAIN_STATE_START;
                break;
            case 5:
                main_state = MAIN_STATE_STOP;
                break;
            case 6:
                if( (main_state == MAIN_STATE_BACKWARD) || (main_state == MAIN_STATE_BACKWARD_CON))
                    main_state = MAIN_STATE_BACKWARD_CON;
                else
                    main_state  = MAIN_STATE_BACKWARD;
                break;
            case 7:
                main_state = MAIN_STATE_STOP;
                break;
            case 8:
                main_state = MAIN_STATE_STOP;
                break;
            case 9:
                main_state = MAIN_STATE_SPEEED_CONTROL;
                break;
            default:
                break;
        }

        switch(main_state){
        case MAIN_STATE_START:
            speedVehicleControlStop();
            statrt_fun_state = 0;
            break;
        case MAIN_STATE_START_CON:
            if(start(FORWARD)){
                speed_sign = 0;
                main_state = MAIN_STATE_NOP;
            }
            break;

        case MAIN_STATE_STOP:
            speedVehicleControlStop();
            if(emergencyFullStop()){
                main_state = MAIN_STATE_NOP;
                speed_sign = 1;
            }
            break;


        case MAIN_STATE_SPEEED_CONTROL:
                speedVehicleControlStart();
                speedSetVehiclePIDReferenceValue(speed_ref);
            break;
        case MAIN_STATE_BACKWARD:
            speedVehicleControlStop();
            statrt_fun_state = 0;
            main_state = MAIN_STATE_BACKWARD_CON;
            break;
        case MAIN_STATE_BACKWARD_CON:
            if(start(BACKWARD)){
                speed_sign = 2;
                main_state = MAIN_STATE_NOP;
            }
            break;
        default:
            break;
        }

        chprintf( (BaseSequentialStream *)&SD3, "Main State: %d\t gear_num: %d\t gear_g_pos: %d\t gear_v_pos: %d\t Clutch_pos: %d\t Break_pos: %d\t speed %.02f\t eng_speed %.02f\t \n\r",main_state, mtControlGetCurrentGearNum(), getGorisontalPosition (), getVerticalPosition(), pedalsClutchGetPosition(),pedalsBrakeGetPosition(), gazelGetSpeed(), gazelGetEngineSpeed());

        chThdSleepMilliseconds(100);
    }


#endif

}

//TODO: Сделать каунтер для i2c error

#include <tests.h>
#include <chprintf.h>
#include <communication.h>


// Main switch state
#define MAIN_STATE_NOP                       0
#define MAIN_STATE_FORWARD                     1
#define MAIN_STATE_STOP                      2
#define MAIN_STATE_HIGHT_SPEEED_CONTROL      3
#define MAIN_STATE_BACKWARD                  4
#define MAIN_STATE_LOW_SPEED_CONTROL         5

#define MAIN_STATE_FORWARD_CON        101
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
 * If gazel have i2c error:
 * 0 - speed > 5 km/h
 *.1 - (-5 < speed < 5) km/h ("0"km/h)
 * 2 - speed < -5 km/h
 *
 * If gazel does not have i2c error:
 *0 >5 km/h
 *1 0>speed>5km/h 
 *2 0
 *3 -5<speed<0 km/h
 *4 <-5 km/h
 *
                           |-----------speed reference (array element number)-----------|
    |current speed         |___0_______|______1_______|___2__|______3_______|______4____|
    (array element number).|
    |      0               | SpeedCont | fuzzy        | Stop |.Stop         | Stop.     |
    |      1               | Start (f) | fuzzy        | Stop | Stop.        | Stop      |
    |      2               | Start (f) | gear=1;fuzzy | Stop |.gear=6;fuzzy |.Start (b) |
    |      3               |.Stop      | Stop.        | Stop | fuzzy        | Start (b) |
    |      4               | Stop      | Stop         | Stop |.fuzzy        |.SpeedCont |

array index corresonds to speed interval (look description before table)
array elements contain numbers from 1 to 25 and detects curent gazel state
(I.g. current gazel state depends on current speed and reference speed.)

 */

static uint8_t arr_fuzzy_err[3][3] = {{1,3,5},{11,13,15},{21,23,25}};
static uint8_t arr_fuzzy_ok[5][5] = {{1,2,3,4,5},{6,7,8,9,10},{11,12,13,14,15},{16,17,18,19,20},{21,22,23,24,25};


void onSet( float speed, float angle ) {
    speed_ref = speed;

    if(gazelGetI2cErrFlag){

        if (speed < 4.0 && speed > -4.0) {
            speed_ref_sign = 1;
        }
        else if ( gazelGetSpeed() < 1.0 ) {

            if(speed > 0)
                speed_ref_sign = 0;
            else
                speed_ref_sign = 2;
        }
    }else{
        if(speed < 5 && speed > -5)

    }

    #endif

    //steerSet...

}

static int8_t statrt_fun_state = 0 ;
//static int16_t _speed = 0;
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

// void  setFun(){

// }
// void  startFun(){

// }
// void  stopFun(){

// }

void testMain( void ){

/*    communicationEventFun_t structForFunc = getDefaultCfg();
 
    structForFunc.on_set    = setFun;
    structForFunc.on_start  = startFun;
    structForFunc.on_stop   = stopFun;

    structForFunc.on_interrupt_timer = NULL;
    
    uint32_t time_for_vt_MS = 2000;

    comm_init(structForFunc, time_for_vt_MS);
*/
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX

    feedbackInit();
    pedalsInit();
    mtControlInit();
    speedInit();
    emergencyStopInit();
    #ifdef FUZZY_LOGIC_ENABLE
        lowSpeedControlIntit();
    #endif

    char  sd_buff[10] = {'?','?','?','?','?','?','?','?','?','?'} ;
    bool start_flag = 0;
    int8_t state = 0;

    while(1){
        // comm_dbgprintf_info("Hellow, int's communication test! ");
        // chThdSleepMilliseconds(1000);

                sdReadTimeout( &SD3, sd_buff, 10, TIME_IMMEDIATE   );

        if(sd_buff[0] == 's')  onSet( atoi(&sd_buff[1]), 0);
        if(sd_buff[0] == 'p')  main_state = MAIN_STATE_STOP;
        // if(sd_buff[0] == 'z')  _speed =  atoi(&sd_buff[1]);

    //    chprintf( (BaseSequentialStream *)&SD3,"state:\t%d\tCl_pos:\t%d\tspeed\t%.2f\tgear: %d\n\r",state,pedalsClutchGetPosition(),gazelGetSpeed(),mtControlGetCurrentGearNum());
        for (int i = 0; i < 9; i++)
        {
        sd_buff[i]='?';
        }

        if(main_state == MAIN_STATE_NOP){
            switch (arr[speed_sign][speed_ref_sign]) {
                case 1:
                    main_state = MAIN_STATE_HIGHT_SPEEED_CONTROL;
                    break;
                case 2:
                    main_state = MAIN_STATE_STOP;
                    break;
                case 3:
                    main_state = MAIN_STATE_STOP;
                    break;
                case 4:
                        main_state  = MAIN_STATE_FORWARD;
                    break;
                case 5:
                        main_state = MAIN_STATE_STOP;
                    break;
                case 6:
                        main_state  = MAIN_STATE_BACKWARD;
                    break;
                case 7:
                    main_state = MAIN_STATE_STOP;
                    break;
                case 8:
                    main_state = MAIN_STATE_STOP;
                    break;
                case 9:
                    main_state = MAIN_STATE_HIGHT_SPEEED_CONTROL;
                    break;
                default:
                    break;
            }
        }

        if( speed_ref_sign == 1 )  main_state = MAIN_STATE_STOP;

        switch(main_state){
        case MAIN_STATE_FORWARD:
            speedVehicleControlStop();
            lowSpeedStop();
            if(emergencyFullStop()){
                statrt_fun_state = 0;
                main_state = MAIN_STATE_FORWARD_CON;
            }
            break;
        case MAIN_STATE_FORWARD_CON:
            if(start(FORWARD)){
                speed_sign = 0;
                main_state = MAIN_STATE_NOP;
            }
            break;

        case MAIN_STATE_STOP:
            speedVehicleControlStop();
            lowSpeedStop();
            if(emergencyFullStop()){
                main_state = MAIN_STATE_NOP;
                speed_sign = 1;
            }
            break;
        case MAIN_STATE_HIGHT_SPEEED_CONTROL:
                lowSpeedStop();
                speedVehicleControlStart();
                if(speed_ref < 0)
                    speedSetVehiclePIDReferenceValue(-speed_ref);
                else
                    speedSetVehiclePIDReferenceValue(speed_ref);
                main_state = MAIN_STATE_NOP;
            break;
        case MAIN_STATE_BACKWARD:
            speedVehicleControlStop();
            lowSpeedStop();
            if(emergencyFullStop()){
                statrt_fun_state = 0;
                main_state = MAIN_STATE_FORWARD_CON;
            }
            break;
        case MAIN_STATE_BACKWARD_CON:
            if(start(BACKWARD)){
                speed_sign = 2;
                main_state = MAIN_STATE_NOP;
            }
            break;
        case MAIN_STATE_LOW_SPEED_CONTROL:
            if(speed_ref < 0)
                lowSpeedSetReference(-speed_ref);
            else
                lowSpeedSetReference(speed_ref);
                lowSpeedStart();
                main_state = MAIN_STATE_NOP;
            break;
        default:
            break;
        }

        chprintf( (BaseSequentialStream *)&SD3, "Main State: %d\t gear_num: %d\t gear_g_pos: %d\t gear_v_pos: %d\t Clutch_pos: %d\t Break_pos: %d\t speed %.02f\t eng_speed %.02f\t \n\r",main_state, mtControlGetCurrentGearNum(), getGorisontalPosition (), getVerticalPosition(), pedalsClutchGetPosition(),pedalsBrakeGetPosition(), gazelGetSpeed(), gazelGetEngineSpeed());

        chThdSleepMilliseconds(100);
    }
}


//TODO: Сделать каунтер для i2c error

#include <tests.h>
#include <chprintf.h>

#include <common.h>


// Main switch state
#define MAIN_STATE_NOP                       0
#define MAIN_STATE_FORWARD                   1
#define MAIN_STATE_STOP                      2
#define MAIN_STATE_HIGHT_SPEEED_CONTROL      3
#define MAIN_STATE_BACKWARD                  4
#define MAIN_STATE_LOW_SPEED_CONTROL         5


#define MAIN_STATE_FORWARD_CON               101
#define MAIN_STATE_BACKWARD_CON              104



//Start funtcion parameters
#define FORWARD  1
#define BACKWARD 6 // 6 gear means backward


bool start( uint8_t dir );

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
static uint8_t arr_fuzzy_ok[5][5] = {{1,2,3,4,5},{6,7,8,9,10},{11,12,13,14,15},{16,17,18,19,20},{21,22,23,24,25}};

//TODO:  what is steer angle variable range?
void commandSet( float speed, float angle ) {
    speed_ref = speed;

    if(gazelGetI2cErrFlag()){
        if (speed < 4.0 && speed > -4.0)
            speed_ref_sign = 1;
        else

            if(speed > 4.0)
                speed_ref_sign = 0;
            else
                speed_ref_sign = 2;


        if ( gazelGetSpeed() < 1.0 )
            if(mtControlGetCurrentGearNum() != 6)
                speed_ref_sign = 0;
            else
                speed_ref_sign = 2;
    }else{
         if(speed < 5 && speed > -5)

            if(speed == 0 )

                speed_ref_sign = 0;
            else
                if(speed < 0)
                    speed_ref_sign = 3;
                else
                    speed_ref_sign = 1;
        if(gazelGetSpeed() < 5)
            if(gazelGetSpeed() == 0)
                speed_sign = 2;
            else
                if(mtControlGetCurrentGearNum() != 6)
                    speed_sign = 1;
                else
                    speed_sign = 3;
        else 
            if(mtControlGetCurrentGearNum() != 6)
                    speed_sign = 0;
                else
                    speed_sign = 4;
    }


    //TODO: ADD Steer control
    steerSetPosition(angle);
}

void commanStop(void){
    //TODO: Command to stop
    engIgnitionSwitchOff();
}

void commanStart(void){
    //TODO: Command to start
    engIgnitionSwitchOn ();
    engStarterSwitchOn ();

}

void connectionErrorCb ( void ){
    comm_dbgprintf_error("Connection fail");
    emergencyFullStop();
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

/* after this time connection error callback is execute*/
#define CONNECTION_FAIL_OK_DELAY 3000  //ms

void testMain( void ){

    feedbackInit();
    pedalsInit();
    mtControlInit();
    speedInit();
    emergencyStopInit();
    lowSpeedControlIntit();
    engIgnitionInit();
    steerInit();

    communicationEventFun_t structForFunc = comm_get_default_cfg();

    structForFunc.on_set    = commandSet;
    structForFunc.on_start  = commanStart;
    structForFunc.on_stop   = commanStop;
    structForFunc.on_interrupt_timer = connectionErrorCb;

    comm_init(&structForFunc, CONNECTION_FAIL_OK_DELAY, true);

    BaseChannel *dbg_chn = comm_get_channel();



    char  sd_buff[10] = {'?','?','?','?','?','?','?','?','?','?'} ;
    bool start_flag = 0;
    int8_t state = 0;

    while(1){


        sdReadTimeout( &SD3, sd_buff, 10, TIME_IMMEDIATE   );

        if(sd_buff[0] == 's')  commandSet( atoi(&sd_buff[1]), 0);
        if(sd_buff[0] == 'p')  main_state = MAIN_STATE_STOP;

        for (int i = 0; i < 9; i++)
        {
        sd_buff[i]='?';
        }

        if(main_state == MAIN_STATE_NOP){
            switch (arr_fuzzy_err[speed_sign][speed_ref_sign]) {
                case 1:
                    main_state = MAIN_STATE_HIGHT_SPEEED_CONTROL;
                    break;
                case 2:
                    main_state = MAIN_STATE_LOW_SPEED_CONTROL;
                    break;
                case 3:
                    main_state = MAIN_STATE_STOP;
                    break;
                case 4:
                    main_state = MAIN_STATE_STOP;
                    break;
                case 5:
                    main_state = MAIN_STATE_STOP;
                    break;
                case 6:
                    MAIN_STATE_FORWARD;
                    break;
                case 7:
                    main_state = MAIN_STATE_LOW_SPEED_CONTROL;
                    break;
                case 8:
                    main_state = MAIN_STATE_STOP;
                    break;
                case 9:
                    main_state = MAIN_STATE_STOP;
                    break;
                case 10:
                    main_state = MAIN_STATE_STOP;
                    break;
                case 11:
                    MAIN_STATE_FORWARD;
                    break;
                case 12:
                    if(mtControlGetCurrentGearNum() != 1){
                        pedalsClutchPress(700);
                        mtControlMannualyShiftGear(1);
                    }
                    else
                        main_state = MAIN_STATE_LOW_SPEED_CONTROL;
                    break;
                case 13:
                    main_state = MAIN_STATE_STOP;
                    break;
                case 14:
                    if(mtControlGetCurrentGearNum() != 6){
                        pedalsClutchPress(700);
                        mtControlMannualyShiftGear(6);
                    }
                    else
                        main_state = MAIN_STATE_LOW_SPEED_CONTROL;
                    break;
                case 15:
                    MAIN_STATE_BACKWARD;
                    break;
                case 16:
                    main_state = MAIN_STATE_STOP;
                    break;
                case 17:
                    main_state = MAIN_STATE_STOP;
                    break;
                case 18:
                    main_state = MAIN_STATE_STOP;
                    break;
                case 19:
                    main_state = MAIN_STATE_LOW_SPEED_CONTROL;
                    break;
                case 20:
                    MAIN_STATE_BACKWARD;
                    break;
                case 21:
                    main_state = MAIN_STATE_STOP;
                    break;
                case 22:
                    main_state = MAIN_STATE_STOP;
                    break;
                case 23:
                    main_state = MAIN_STATE_STOP;
                    break;
                case 24:
                    main_state = MAIN_STATE_LOW_SPEED_CONTROL;
                    break;
                case 25:
                    main_state = MAIN_STATE_HIGHT_SPEEED_CONTROL;
                    break;
                default:
                    break;
            }
        }

        if( speed_ref == 0 )  main_state = MAIN_STATE_STOP;

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

        comm_dbgprintf("Main State: %d\t gear_num: %d\t Clutch_pos: %d\t ",main_state, mtControlGetCurrentGearNum(), pedalsClutchGetPosition());
        comm_dbgprintf("Break_pos: %d\t speed %.02f\t eng_speed %.02f\t \n\r",pedalsBrakeGetPosition(), gazelGetSpeed(), gazelGetEngineSpeed());
        if(gazelGetI2cErrFlag())
            comm_dbgprintf("I2c error!\r\n");
        chThdSleepMilliseconds(100);
    }
}


#include <tests.h>
#include <lld_can.h>
#include <lld_ext_dac.h>
#include <controllers.h>
#include <lld_control.h>
#include <chprintf.h>
#include <mt_control.h>





void RisingEdgeClutchMCallback(PWMDriver *pwmd);

void fallingEdgeClutchMCallback(PWMDriver *pwmd);

void RisingEdgeBreakMCallback(PWMDriver *pwmd);

void fallingEdgeBreakMCallback(PWMDriver *pwmd);



//float       EngineSpeed = 770.0;
uint8_t     val = 55;  //50 = 1V
float       esV = 0;
bool        control_start = 0;
uint8_t CSErrorDeadzoneHalfwidth = 1;
extern  gazelParam gazel;

static PIDControllerContext_t  pidCtxV = {
    .kp   = 0.0,
    .ki   = 0.0,
    .kd   = 0,
    .integrLimit  = 100,
    .integZone = 0.0
};

MotorDriver ClutchM = {
    .pwmd            =   &PWMD3,
    .dir_line        =   PAL_LINE(GPIOB, 8),
    .rising_edge_cb  =   RisingEdgeClutchMCallback,
    .falling_edge_cb =   fallingEdgeClutchMCallback,
    .max_position    =   90000,
    .min_position     =  0
};



void RisingEdgeClutchMCallback(PWMDriver *pwmd){

    (void) pwmd;
    risingEdgeCb(&ClutchM);

}
void fallingEdgeClutchMCallback(PWMDriver *pwmd){

    (void) pwmd;
    fallingEdgeCb(&ClutchM);
   // palToggleLine(PAL_LINE(GPIOB,14));
//Code when motor must stop
}

MotorDriver BreakM = {
    .pwmd            =   &PWMD4,
    .dir_line        =   PAL_LINE(GPIOD, 11),
    .rising_edge_cb  =   RisingEdgeBreakMCallback,
    .falling_edge_cb =   fallingEdgeBreakMCallback,
    .max_position    =   25000,
    .min_position     =  0
};



void RisingEdgeBreakMCallback(PWMDriver *pwmd){

    (void) pwmd;
    risingEdgeCb(&BreakM);

}
void fallingEdgeBreakMCallback(PWMDriver *pwmd){

    (void) pwmd;
    fallingEdgeCb(&BreakM);
  //  palToggleLine(PAL_LINE(GPIOB,7));
//Code when motor must stop
}



<<<<<<< HEAD
<<<<<<< HEAD
/**********************************************PID*****************************************************/


#define VEHICLE_SPEED_REFERENCE_ACCELERATION        0.5 /* (km/h)/100ms   */

=======
>>>>>>> parent of 40e58b8... [MT_control]: Добавлена синхорнизация
=======
>>>>>>> parent of 40e58b8... [MT_control]: Добавлена синхорнизация
uint32_t map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


<<<<<<< HEAD
<<<<<<< HEAD
static uint8_t     val = 55;  //50 = 1V
static float       Eref = 770.0;
static float       Vref = 0;
static float       current_Vref = 0;


static int32_t VehicleControl = 0;
static int32_t engine_control_value =0;
static uint8_t CSErrorDeadzoneHalfwidth = 1;

static bool        engine_control_start = 0;
static bool        vehicle_control_start = 0;
static bool        speed_reference_linear_acceleration_enable = 0;

static PIDControllerContext_t  pidCtx = {
    .kp   = 3.5,
    .ki   = 0.06,
    .kd   = 0,
    .integrLimit  = 5000,
    .integZone = 0.5
};

static PIDControllerContext_t  pidCtxV = {
    .kp   = 3.5,
    .ki   = 0.7,
    .kd   = 0,
    .integrLimit  = 100,
    .integZone = 0.9
=======
static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
>>>>>>> parent of 40e58b8... [MT_control]: Добавлена синхорнизация
=======
static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
>>>>>>> parent of 40e58b8... [MT_control]: Добавлена синхорнизация
};

/**
 * @brief   Control system of steering implementation
 * @param   position        - reference position
 * @return  controlValue    - control signal value [-100 100] ( in percent )
 */


int32_t VehicleControl = 0;
uint32_t vehicleSpeedControl( uint32_t speed )
{

    //engine_speed_rpm  = CLIP_VALUE( engine_speed_rpm, 0, 100 );
    speed = map(speed,0,100,0,1000);

    //engine_speed_rpm  = CLIP_VALUE( engine_speed_rpm, 0, 100 );

    int16_t current_speed = map(gazel.Speed,0,100,0,1000);

    int16_t error = speed - current_speed;

    /* Dead zone for (p) error */
    if ( abs(error) < CSErrorDeadzoneHalfwidth )
    {
        pidCtxV.err = 0;
    }
    else
    {
        pidCtxV.err = error;
    }
    pidCtxV.integZone_abs = speed * pidCtxV.integZone;
    VehicleControl    = (int32_t) PIDControlResponse( &pidCtxV );


    /*  roughly reset integral */
    VehicleControl = CLIP_VALUE(VehicleControl,0,1000);
    VehicleControl = map(VehicleControl,0,1000,77,220);

    return VehicleControl;

}


static THD_WORKING_AREA(pid_wa, 64);
static THD_FUNCTION(pid, arg) {

    (void)arg;

    PIDControlInit( &pidCtxV );
    palSetPad(GPIOB,0);

    while(1){
<<<<<<< HEAD
<<<<<<< HEAD

        if((gazel.BrakeSwitch==1) || (gazel.ClutchSwitch==1)) vehicle_control_start = false;



        if ((engine_control_start )  && (!vehicle_control_start)  ){
            val = engineSpeedControl(( uint32_t ) Eref);
        }
        else{
                pidCtx.err        = 0;
                pidCtx.prevErr    = 0;
                pidCtx.integrSum  = 0;
                engine_control_value = 0;
        }



        if ( (vehicle_control_start) && (!engine_control_start ) )
        {
//            if (( Vref > gazel.Speed + VEHICLE_SPEED_REFERENCE_ACCELERATION ) && (speed_reference_linear_acceleration_enable))
//            {
//                current_Vref = gazel.Speed + VEHICLE_SPEED_REFERENCE_ACCELERATION;
//            }
//            else
//            {
//                current_Vref = Vref;
//            }
            val = vehicleSpeedControl((uint32_t) Vref);
=======
        if (control_start && (gazel.BrakeSwitch==0) && (gazel.ClutchSwitch==0)){
            val = vehicleSpeedControl((uint32_t) esV);
>>>>>>> parent of 40e58b8... [MT_control]: Добавлена синхорнизация
=======
        if (control_start && (gazel.BrakeSwitch==0) && (gazel.ClutchSwitch==0)){
            val = vehicleSpeedControl((uint32_t) esV);
>>>>>>> parent of 40e58b8... [MT_control]: Добавлена синхорнизация
        }
        else{
                pidCtxV.err        = 0;
                pidCtxV.prevErr    = 0;
                pidCtxV.integrSum  = 0;
                VehicleControl = 0;
                val = 58;


        }
<<<<<<< HEAD
<<<<<<< HEAD

        val = val < 58 ? 58 : val;
        extDacSetValue(( uint8_t)(val*0.55),val);
=======
>>>>>>> parent of 40e58b8... [MT_control]: Добавлена синхорнизация
=======
>>>>>>> parent of 40e58b8... [MT_control]: Добавлена синхорнизация

        extDacSetValue(( uint8_t)(val*0.55),val);
        chThdSleepMilliseconds( 100 );

    }
}

<<<<<<< HEAD
<<<<<<< HEAD




/*********************************************************** MT Control **********************************************/

#define ENGINE_SPEED_THRESHOLD 1900

=======
>>>>>>> parent of 40e58b8... [MT_control]: Добавлена синхорнизация
=======
>>>>>>> parent of 40e58b8... [MT_control]: Добавлена синхорнизация
bool gear_shift_control = 0;
int8_t gear_num = 0;
uint8_t gear = -1;
float eng_speed_debug = 0;
bool shift_enable_flag = 0;

<<<<<<< HEAD
<<<<<<< HEAD
uint8_t synchronization_coeff = 0;

void gearControl ( uint8_t target_gear )
{
    switch(target_gear){
        case 2: synchronization_coeff = 75.3; break;
        case 3: synchronization_coeff = 49.0; break;
    }
    if ( gear_num == target_gear - 1  )
    {
        MotorRunContinuous( &ClutchM, 0, 500 );
        if ( ClutchM.state == 0 )
        {
            gear = target_gear;
            Eref = gazel.Speed * synchronization_coeff;
            engine_control_start = 1;
        }
    }
    if ( ( gear_num == target_gear ) )
    {
        MotorRunContinuous( &ClutchM, 1, 500 );
        if ( ClutchM.state == 0 )
        {
            shift_enable_flag = 0;
            engine_control_start = false;
            vehicle_control_start = true;
        }

    }

}

static THD_WORKING_AREA(gearshift_wa, 256);
=======
static THD_WORKING_AREA(gearshift_wa, 128);
>>>>>>> parent of 40e58b8... [MT_control]: Добавлена синхорнизация
=======
static THD_WORKING_AREA(gearshift_wa, 128);
>>>>>>> parent of 40e58b8... [MT_control]: Добавлена синхорнизация
static THD_FUNCTION(gearshift, arg) {

    (void)arg;
    while(1){
        switch(gear){
            case 0: gear_num = shiftMTToNeutral(1000); break;
            case 1: gear_num = shiftMTToNextGear(1,1000); break;
            case 2: gear_num = shiftMTToNextGear(2,1000); break;
            case 3: gear_num = shiftMTToNextGear(3,1000); break;
            case 6: gear_num = shiftMTToNextGear(6,1000); break;
        }

<<<<<<< HEAD
<<<<<<< HEAD
            if ( ( gazel.EngineSpeed >= ENGINE_SPEED_THRESHOLD ) && (gear_shift_control == 1) ){
                shift_enable_flag = 1;
            }
                if ( shift_enable_flag ){
                    gearControl( gear_num + 1 );
                }

=======
=======
>>>>>>> parent of 40e58b8... [MT_control]: Добавлена синхорнизация
            if ( ( gazel.EngineSpeed >= 1300 ) && (gear_shift_control == 1) && ( gear_num != 2 ) )
            //if ( ( eng_speed_debug >= 1200 ) && (gear_shift_control == 1) )
            {
                shift_enable_flag = 1;
            }

            if ( shift_enable_flag )
            {
                if ( gear_num != 2  )
                {
                    MotorRunContinuous( &ClutchM, 0, 500 );
                    if ( ClutchM.state == 0 )
                    {
                        gear = 2;
                    }
                }
                if ( gear_num == 2  )
                {
                    MotorRunContinuous( &ClutchM, 1, 1500 );
                    shift_enable_flag = 0;
                }
            }
>>>>>>> parent of 40e58b8... [MT_control]: Добавлена синхорнизация

            chThdSleepMilliseconds( 50 );
    }

}

void TestMtControl ( void )
{
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX

    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );   //Led

    can_init();
    extDacInit();
    chThdCreateStatic(pid_wa, sizeof(pid_wa), NORMALPRIO + 10, pid, NULL);
    chThdCreateStatic(gearshift_wa, sizeof(gearshift_wa), NORMALPRIO + 7, gearshift, NULL);

      /*MotoR driver Setting */
    palSetLineMode( PAL_LINE( GPIOC, 6),  PAL_MODE_ALTERNATE(2) );
    palSetLineMode( ClutchM.dir_line, PAL_MODE_OUTPUT_PUSHPULL);
    MotorlldControlInit( &ClutchM );

    palSetLineMode( PAL_LINE( GPIOD, 12),  PAL_MODE_ALTERNATE(2) );
    palSetLineMode( BreakM.dir_line, PAL_MODE_OUTPUT_PUSHPULL);
    MotorlldControlInit( &BreakM );

    mtControlInit ();




    uint16_t speed = 4000;
    static char  sd_buff[10] ;
//    bool vupLS_state = 0, vlowLS_state = 0, hlLS_state = 0, hrLS_state = 0;
//    uint8_t all_sensors_state = 0;

    while(1) {

        sdReadTimeout( &SD3, sd_buff, 9, TIME_IMMEDIATE );

        if(sd_buff[5]=='n') {
          speed = atoi(sd_buff);
          MotorSetSpeed( &ClutchM, speed);
        }

        //*****MOTOR CONTROL*******//
        if(sd_buff[0]=='u') { MotorRunTracking( &ClutchM, speed); MotorRunTracking( &BreakM, speed);}

        if(sd_buff[6]=='q')
        {
            gear = -1;
            setTrackedModePositionVerticalMotor ( atoi(sd_buff) );
        }

        if(sd_buff[6]=='w')
        {
            gear = -1;
            setTrackedModePositionGorisontalMotor ( atoi(sd_buff) );
        }

        if(sd_buff[0]=='c') { MotorStop( &ClutchM );  MotorStop( &BreakM ); }
        if(sd_buff[0]=='z') MotorRunContinuous(&ClutchM, 0, 1000);
        if(sd_buff[0]=='o') MotorRunContinuous(&ClutchM, 1, 1000);
        if(sd_buff[0]=='x') MotorRunContinuous(&BreakM, 0, 1000);
        if(sd_buff[0]=='f') MotorRunContinuous(&BreakM, 1, 1000);


        //*****PID CONTROL*******//
<<<<<<< HEAD
<<<<<<< HEAD

        if(sd_buff[5]=='b') Vref = atoi(sd_buff);
        if(sd_buff[0]=='a') vehicle_control_start = 1;
        if(sd_buff[0]=='s') vehicle_control_start = 0;

        if(sd_buff[0]=='j') speed_reference_linear_acceleration_enable = !speed_reference_linear_acceleration_enable;
        if(sd_buff[0]=='l') engine_control_start = 0;


=======
=======
>>>>>>> parent of 40e58b8... [MT_control]: Добавлена синхорнизация
        if(sd_buff[5]=='p') pidCtxV.kp = atoi(sd_buff)/100.0;
        if(sd_buff[5]=='i') pidCtxV.ki = atoi(sd_buff)/1000.0;
        if(sd_buff[5]=='m') pidCtxV.kd = atoi(sd_buff)/1000.0;
        if(sd_buff[5]=='v') { control_start = 0; val = atoi(sd_buff); }
        if(sd_buff[0]=='s') { control_start = 0; val = 55; }
        if(sd_buff[5]=='b') esV = atoi(sd_buff);
        if(sd_buff[0]=='a') control_start = 1;
        if(sd_buff[2]=='j') pidCtxV.integZone = atoi(sd_buff)/100.0;
        if(sd_buff[5]=='l') pidCtxV.integrLimit = atoi(sd_buff);
        if(sd_buff[5]=='k') CSErrorDeadzoneHalfwidth = atoi(sd_buff);
<<<<<<< HEAD
>>>>>>> parent of 40e58b8... [MT_control]: Добавлена синхорнизация
=======
>>>>>>> parent of 40e58b8... [MT_control]: Добавлена синхорнизация


        //**** Gear Shift commands ****//
        if(sd_buff[0]=='e') gear = 0;
        if(sd_buff[0]=='d') gear = 1;
        if(sd_buff[0]=='r') gear = 2;
        if(sd_buff[0]=='k') gear = 3;
        if(sd_buff[0]=='g') gear = 6;
        if(sd_buff[0]=='y') gear_shift_control = 1;
        if(sd_buff[0]=='h') gear_shift_control = 0;
        if(sd_buff[5]=='t') eng_speed_debug = atoi(sd_buff);

<<<<<<< HEAD
<<<<<<< HEAD
        //uint8_t vm_state = getVerticalState();
        //uint8_t gm_state = getGorisontalState ();
        int32_t v_motor_position = getVerticalPosition();
        int32_t g_motor_position = getGorisontalPosition();

        //chprintf( (BaseSequentialStream *)&SD3, "err: %.2f Control: %d A: %d Pedal: %.1f ESpeed: %.02f  VSeed: %.2f ________ Kp: %.02f ki: %.04f Kd:%.02f  ISum: %.3f ______INTEGZONE: %.3f  GearControl %d ClutchState %d  \r\n", pidCtxV.err, VehicleControl, (uint8_t)val, gazel.AcceleratorPedalPosition, gazel.EngineSpeed, gazel.Speed , pidCtxV.kp, pidCtxV.ki, pidCtxV.kd,pidCtxV.integrSum, pidCtxV.integZone_abs, gear, ClutchM.state );

        chprintf( (BaseSequentialStream *)&SD3, "EControlEn %d\tGControlEn %d\tVControlEn %d\tVref: %.2f\tRefLinEn %d\tVspeed: %.2f\tESpeed: %.02f\tGN %d\tVpos %d\tGpos %d\tCS %d\tBS %d\tClPos %d\t\r\n", engine_control_start, gear_shift_control, vehicle_control_start, Vref,speed_reference_linear_acceleration_enable, gazel.Speed, gazel.EngineSpeed, gear_num, v_motor_position, g_motor_position, gazel.ClutchSwitch, gazel.BrakeSwitch, ClutchM.position );


=======
        chprintf( (BaseSequentialStream *)&SD3, "err: %.2f Control: %d A: %d Pedal: %.1f ESpeed: %.02f  VSeed: %.2f ________ Kp: %.02f ki: %.04f Kd:%.02f  ISum: %.3f ______INTEGZONE: %.3f  GearControl %d ClutchState %d  \r\n", pidCtxV.err, VehicleControl, (uint8_t)val, gazel.AcceleratorPedalPosition, gazel.EngineSpeed, gazel.Speed , pidCtxV.kp, pidCtxV.ki, pidCtxV.kd,pidCtxV.integrSum, pidCtxV.integZone_abs, gear, ClutchM.state );
>>>>>>> parent of 40e58b8... [MT_control]: Добавлена синхорнизация
=======
        chprintf( (BaseSequentialStream *)&SD3, "err: %.2f Control: %d A: %d Pedal: %.1f ESpeed: %.02f  VSeed: %.2f ________ Kp: %.02f ki: %.04f Kd:%.02f  ISum: %.3f ______INTEGZONE: %.3f  GearControl %d ClutchState %d  \r\n", pidCtxV.err, VehicleControl, (uint8_t)val, gazel.AcceleratorPedalPosition, gazel.EngineSpeed, gazel.Speed , pidCtxV.kp, pidCtxV.ki, pidCtxV.kd,pidCtxV.integrSum, pidCtxV.integZone_abs, gear, ClutchM.state );
>>>>>>> parent of 40e58b8... [MT_control]: Добавлена синхорнизация

        //*****Calibration testing*****//
        #if 1

        //if(sd_buff[0]=='e') all_sensors_state = doCalibrationMT ();

        //all_sensors_state = doCalibrationMT ();
        uint8_t vm_state = getVerticalState();
        uint8_t gm_state = getGorisontalState ();
        int32_t v_motor_position = getVerticalPosition();
        int32_t g_motor_position = getGorisontalPosition();


     // chprintf( (BaseSequentialStream *)&SD3, "vupLS_state:  %d vlowLS_state:  %d hlLS_state:  %d hrLS_state :  %d  v_pos:  %d  h_pos:  %d  vm_state: %d hm_state: %d \r\n",(all_sensors_state)&&(1<<3),(all_sensors_state)&&(1<<2),(all_sensors_state)&&(1<<1) ,(all_sensors_state)&&(1<<0) , v_motor_position, g_motor_position, vm_state, gm_state);
        #endif


        for (int i = 0; i < 9; i++)
        {
          sd_buff[i]='?';
         // sd_buff2[i]='?';
        }
        chThdSleepMilliseconds( 500 );
    }
}














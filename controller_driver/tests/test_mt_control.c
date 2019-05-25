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
    .max_position    =   92000,
    .min_position     =  0
};



void RisingEdgeClutchMCallback(PWMDriver *pwmd){

    (void) pwmd;
    risingEdgeCb(&ClutchM);

}
void fallingEdgeClutchMCallback(PWMDriver *pwmd){

    (void) pwmd;
    fallingEdgeCb(&ClutchM);
    palToggleLine(PAL_LINE(GPIOB,14));
//Code when motor must stop
}

MotorDriver BreakM = {
    .pwmd            =   &PWMD4,
    .dir_line        =   PAL_LINE(GPIOD, 11),
    .rising_edge_cb  =   RisingEdgeBreakMCallback,
    .falling_edge_cb =   fallingEdgeBreakMCallback,
    .max_position    =   20000,
    .min_position     =  0
};



void RisingEdgeBreakMCallback(PWMDriver *pwmd){

    (void) pwmd;
    risingEdgeCb(&BreakM);

}
void fallingEdgeBreakMCallback(PWMDriver *pwmd){

    (void) pwmd;
    fallingEdgeCb(&BreakM);
    palToggleLine(PAL_LINE(GPIOB,7));
//Code when motor must stop
}



uint32_t map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
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
    VehicleControl = map(VehicleControl,0,1000,77,240);

    return VehicleControl;

}


static THD_WORKING_AREA(pid_wa, 64);
static THD_FUNCTION(pid, arg) {

    (void)arg;

    PIDControlInit( &pidCtxV );
    palSetPad(GPIOB,0);

    while(1){
        if (control_start && (gazel.BrakeSwitch==0) && (gazel.ClutchSwitch==0)){
            val = vehicleSpeedControl((uint32_t) esV);
        }
        else{
                pidCtxV.err        = 0;
                pidCtxV.prevErr    = 0;
                pidCtxV.integrSum  = 0;
                VehicleControl = 0;
                val = 58;


        }

        extDacSetValue(( uint8_t)(val*0.55),val);
        chThdSleepMilliseconds( 100 );

    }
}

bool gear_shift_control = 0;
int8_t gear_num = 0;
uint8_t gear = -1;
float eng_speed_debug = 0;
bool shift_enable_flag = 0;

static THD_WORKING_AREA(gearshift_wa, 128);
static THD_FUNCTION(gearshift, arg) {

    (void)arg;
    while(1){
        switch(gear){
        case 0: gear_num = shiftMTToNeutral(1000); break;
        case 1: gear_num = shiftMTToNextGear(1,1000); break;
        case 2: gear_num = shiftMTToNextGear(2,1000); break;
        case 6: gear_num = shiftMTToNextGear(6,1000); break;
        }

            if ( ( gazel.EngineSpeed >= 1500 ) && (gear_shift_control == 1) && ( gear_num != 2 ) )
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


    uint32_t CPSpeed = 5000;
    uint32_t steps = 4000;
    uint16_t speed = 4000;

    static char  sd_buff[10] ;
   // static char  sd_buff2[10] ;

    while(1) {

        sdReadTimeout( &SD3, sd_buff, 9, TIME_IMMEDIATE );

        //if(sd_buff[6]=='q') ClutchM.tracked_position = atoi(sd_buff);
        //if(sd_buff[6]=='w') BreakM.tracked_position = atoi(sd_buff);

        if(sd_buff[5]=='n') {
          speed = atoi(sd_buff);
          MotorSetSpeed( &ClutchM, speed);
        }

        //*****MOTOR CONTROL*******//

        //if(sd_buff[0]=='r') MotorRunContinuous( &ClutchM, 1, speed);
        //if(sd_buff[0]=='f') MotorRunContinuous( &ClutchM, 0, speed);
        //if(sd_buff[0]=='t') MotorRunContinuous( &BreakM, 1, speed);
        //if(sd_buff[0]=='g') MotorRunContinuous( &BreakM, 0, speed);

        if(sd_buff[0]=='u') { MotorRunTracking( &ClutchM, speed); MotorRunTracking( &BreakM, speed);}

        if(sd_buff[6]=='q') {
                                gear = -1;
                                setTrackedModePositionVerticalMotor ( atoi(sd_buff) ); //ClutchM.tracked_position = atoi(sd_buff);
                            }

        if(sd_buff[6]=='w')
            {
                gear = -1;
                setTrackedModePositionGorisontalMotor ( atoi(sd_buff) );
            }

        if(sd_buff[0]=='c') { MotorStop( &ClutchM );  MotorStop( &BreakM ); }

        if(sd_buff[5]=='z') ClutchM.max_position = atoi(sd_buff);
        if(sd_buff[5]=='x') BreakM.max_position = atoi(sd_buff);


        //*****PID CONTROL*******//

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


        //**** Gear Shift command ****//

        if(sd_buff[0]=='e') gear = 0;
        if(sd_buff[0]=='d') gear = 1;
        if(sd_buff[0]=='r') gear = 2;
        if(sd_buff[0]=='g') gear = 6;
        if(sd_buff[0]=='y') gear_shift_control = 1;
        if(sd_buff[0]=='h') gear_shift_control = 0;
        if(sd_buff[5]=='t') eng_speed_debug = atoi(sd_buff);

        //chprintf( (BaseSequentialStream *)&SD3, "Clutch: %d Brake: %d \r\n",gazel.ClutchSwitch, gazel.BrakeSwitch );
        chprintf( (BaseSequentialStream *)&SD3, "err: %.2f Control: %d A: %d Pedal: %.1f ESpeed: %.02f  VSeed: %.2f ________ Kp: %.02f ki: %.04f Kd:%.02f  ISum: %.3f ______INTEGZONE: %.3f  GearControl %d ClutchState %d  \r\n", pidCtxV.err, VehicleControl, (uint8_t)val, gazel.AcceleratorPedalPosition, gazel.EngineSpeed, gazel.Speed , pidCtxV.kp, pidCtxV.ki, pidCtxV.kd,pidCtxV.integrSum, pidCtxV.integZone_abs, gear, ClutchM.state );
        //chprintf( (BaseSequentialStream *)&SD7,"{%d,%d,%d,%d}",ClutchM.position,BreakM.position,speed,speed);

        for (int i = 0; i < 9; i++)
        {
          sd_buff[i]='?';
         // sd_buff2[i]='?';
        }
        chThdSleepMilliseconds( 500 );
    }
}


























///****************************************************************************
#if 0
#include <tests.h>
#include <lld_control.h>
#include <chprintf.h>
#include <mt_control.h>


static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};






void TestMtControl ( void )
{
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX


    mtControlInit ();

    static char  sd_buff[10] ;
    uint16_t speed = 10000;

    int32_t     v_pos = 0,
                g_pos = 0,
                v_max_pos = 0,
                g_max_pos = 0,
                v_target_pos = 0,
                g_target_pos = 0;

    bool        v_state = false,
                g_state = false;

    uint8_t     v_mode = 0,
                g_mode = 0;

    uint16_t    v_speed = 0,
                g_speed = 0;

    char        command = ' ';



    while(1)
    {

        sdReadTimeout( &SD3, sd_buff, 7, TIME_IMMEDIATE   );

        v_pos        = getVerticalPosition ();
        g_pos        = getGorisontalPosition ();
        v_max_pos    = getVerticalMaxPosition ();
        g_max_pos    = getGorisontalMaxPosition ();
        v_target_pos = getVerticalTargetPosition ();
        g_target_pos = getGorisontalTargetPosition ();
        v_state      = getVerticalState ();
        g_state      = getGorisontalState ();
        v_mode       = getVerticalMode ();
        g_mode       = getGorisontalMode ();
        v_speed      = getVerticalSpeed ();
        g_speed      = getGorisontalSpeed ();


//        //*****MOTOR CONTROL*******//
//
//        if(sd_buff[5]=='v') {
//          speed = atoi(sd_buff);
//          MotorSetSpeed( &m_vertical, speed);
//        }
//
//
//
//        if(sd_buff[0]=='r') MotorRunContinuous( &m_vertical, 1, speed);
//        if(sd_buff[0]=='f') MotorRunContinuous( &m_vertical, 0, speed);
//        if(sd_buff[0]=='t') MotorRunContinuous( &m_gorisontal, 1, speed);
//        if(sd_buff[0]=='g') MotorRunContinuous( &m_gorisontal, 0, speed);
//
        if(sd_buff[0]=='u') setTrackedMode( speed, speed );
//
        if(sd_buff[6]=='q') setTrackedModePositionVerticalMotor ( atoi(sd_buff) );
        if(sd_buff[6]=='w') setTrackedModePositionGorisontalMotor ( atoi(sd_buff) );
//
        if(sd_buff[0]=='c') {  verticalMotorStop ();  gorisontalMotorStop (); }
//
//
        if(sd_buff[6]=='e') { command = ' '; verticalCaclibration (1, speed, atoi(sd_buff));}
        if(sd_buff[6]=='d') { command = ' '; verticalCaclibration (0, speed, atoi(sd_buff));}
        if(sd_buff[6]=='y') { command = ' '; gorisontalCaclibration (1, speed, atoi(sd_buff));}
        if(sd_buff[6]=='h') { command = ' '; gorisontalCaclibration (0, speed, atoi(sd_buff));}

        if(sd_buff[6]=='z') setVerticalMotorMaxPosition ( atoi(sd_buff) ) ;
        if(sd_buff[6]=='x') setGorisontalMotorMaxPosition ( atoi(sd_buff) );



        /*
         * (0,0)-neutral
         * 1 gear (vertical) = 12500 // 1 gear (gorisontal) = 13000
         * 2 gear (gorisontal) = -17000
         * 3 gear   (gorisontal) = 13000
         *
         *
         * */

        /* Gear selection */
        if(sd_buff[0]=='n')
        {
            command = 'n';
            //shiftMTToNeutral ();
        }

        if(sd_buff[0]=='i')
        {
            command = '1';
            //shiftToFirstGear ();
        }

        if(sd_buff[0]=='o')
        {
            command = '2';
            //shiftToSecondGear ();
        }

        if(sd_buff[0]=='k')
        {
            command = '3';
            //shiftToThirdGear ();
        }

        if(sd_buff[0]=='l')
        {
            command = '4';
            //shiftToForthGear ();
        }

        if(sd_buff[0]=='m')
        {
            command = '5';
            //shiftToFifthGear ();
        }

        if(sd_buff[0]=='p') // back
        {
            command = 'p';
            //shiftToReverseGear ();
        }

        switch ( command )
        {
            case 'n':   shiftMTToNeutral ( 1000 ); break;
            case '1':   shiftMTToNextGear (1, 1000); break;
            case '2':   shiftMTToNextGear (2, 1000); break;
            case '3':   shiftMTToNextGear (3, 1000); break;
            case '4':   shiftMTToNextGear (4, 1000); break;
            case '5':   shiftMTToNextGear (5, 1000); break;
            case 'p':   shiftMTToNextGear (6, 1000); break;

        }



        chprintf( (BaseSequentialStream *)&SD3, "State_v: %d State_g: %d  Mode_v: %d Mode_g: %d  "
                "Position_v: %d  Position_g: %d Max_v: %d Max_g: %d Track_v: %d Track_g: %d command %c\r\n",
                v_state,g_state , v_mode,g_mode, v_pos ,g_pos ,v_max_pos ,
                g_max_pos , v_target_pos, g_target_pos, command);

        for (int i = 0; i < 9; i++)
       {
         sd_buff[i]='?';
       }
        chThdSleepMilliseconds( 100 );


    };
}

#endif

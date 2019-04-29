#include <tests.h>
#include <lld_can.h>
#include <lld_ext_dac.h>
#include <controllers.h>
#include <lld_control.h>
#include <chprintf.h>





void RisingEdgeClutchMCallback(PWMDriver *pwmd);

void fallingEdgeClutchMCallback(PWMDriver *pwmd);

void RisingEdgeBreakMCallback(PWMDriver *pwmd);

void fallingEdgeBreakMCallback(PWMDriver *pwmd);



//float       EngineSpeed = 770.0;
uint8_t     val = 55;  //50 = 1V
float       es = 770.0;
bool        control_start = 0;
uint8_t CSErrorDeadzoneHalfwidth = 0;
extern  gazelParam gazel;

static PIDControllerContext_t  pidCtx = {
    .kp   = 1,
    .ki   = 0.01,
    .kd   = 0,
    .integrLimit  = 8000
};

MotorDriver ClutchM = {
    .pwmd            =   &PWMD3,
    .dir_line        =   PAL_LINE(GPIOB, 8),
    .rising_edge_cb  =   RisingEdgeClutchMCallback,
    .falling_edge_cb =   fallingEdgeClutchMCallback,
    .max_position    =   1000
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
    .max_position    =   1000
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
uint32_t engineSpeedControl( uint32_t engine_speed_rpm )
{

    engine_speed_rpm = map(engine_speed_rpm,0,5000,0,5000);

    //engine_speed_rpm  = CLIP_VALUE( engine_speed_rpm, 0, 100 );

    int16_t current_engine_speed = map(gazel.EngineSpeed,0,5000,0,5000);

    int16_t error = engine_speed_rpm - current_engine_speed;
    
    /* Dead zone for (p) error */
    if ( abs(error) < CSErrorDeadzoneHalfwidth )
    {
        pidCtx.err = 0;
    }
    else
    {
        pidCtx.err = error;
    }

    uint32_t controlValue    = PIDControlResponse( &pidCtx );


    /*  roughly reset integral */
    controlValue = CLIP_VALUE(controlValue,0,420);// 55 -> 215; 107 -> 420
    controlValue = map(controlValue,0,5000,77,255);

    return controlValue;

}

static THD_WORKING_AREA(pid_wa, 64);
static THD_FUNCTION(pid, arg) {
    
    (void)arg;

    PIDControlInit( &pidCtx );
    palSetPad(GPIOB,0);
    
    while(1){
        if (control_start ){
            val = engineSpeedControl(( uint32_t ) es);
        }
        else{
            //val = 55;
                pidCtx.err        = 0;
                pidCtx.prevErr    = 0;
                pidCtx.integrSum  = 0;
        }
        

        chThdSleepMilliseconds( 20 );
    }
}

void TestEngineSpeedRouting ( void )
{
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX

    sdStart( &SD7, &sdcfg );
    palSetPadMode( GPIOD, 3, PAL_MODE_ALTERNATE(12) );   // TX
    palSetPadMode( GPIOD, 4, PAL_MODE_ALTERNATE(12) );   // RX



    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );   //Led
    can_init();
    extDacInit();
    chThdCreateStatic(pid_wa, sizeof(pid_wa), NORMALPRIO + 10, pid, NULL);

      /*MotoR driver Setting */
    palSetLineMode( PAL_LINE( GPIOC, 6),  PAL_MODE_ALTERNATE(2) );
    palSetLineMode( ClutchM.dir_line, PAL_MODE_OUTPUT_PUSHPULL);

    MotorlldControlInit( &ClutchM );

    palSetLineMode( PAL_LINE( GPIOD, 12),  PAL_MODE_ALTERNATE(2) );
    palSetLineMode( BreakM.dir_line, PAL_MODE_OUTPUT_PUSHPULL);
    MotorlldControlInit( &BreakM );


    uint32_t CPSpeed = 5000;
    uint32_t steps = 4000;
    uint16_t speed = 4000;

    static char  sd_buff[10] ;
    static char  sd_buff2[10] ;

    while(1) {

        sdReadTimeout( &SD3, sd_buff, 6, TIME_IMMEDIATE   );
        //sdReadTimeout( &SD7, sd_buff2, 6, TIME_IMMEDIATE   );
        //sd_buff2[9] = 0;

        if(sd_buff2[5]=='q') ClutchM.tracked_position = atoi(sd_buff);
        if(sd_buff2[5]=='w') BreakM.tracked_position = atoi(sd_buff);

        if(sd_buff[5]=='n') {
          speed = atoi(sd_buff);
          MotorSetSpeed( &ClutchM, speed); 
        }

        //*****MOTOR CONTROL*******//

        if(sd_buff[0]=='r') MotorRunContinuous( &ClutchM, 1, speed);
        if(sd_buff[0]=='f') MotorRunContinuous( &ClutchM, 0, speed);
        if(sd_buff[0]=='t') MotorRunContinuous( &BreakM, 1, speed);
        if(sd_buff[0]=='g') MotorRunContinuous( &BreakM, 0, speed);

        if(sd_buff[0]=='u') { MotorRunTracking( &ClutchM, speed); MotorRunTracking( &BreakM, speed);}

        if(sd_buff[5]=='q') ClutchM.tracked_position = atoi(sd_buff);
        if(sd_buff[5]=='w') BreakM.tracked_position = atoi(sd_buff);

        if(sd_buff[0]=='c') { MotorStop( &ClutchM );  MotorStop( &BreakM ); }


        if(sd_buff[5]=='e') MotorRunCaclibration( &ClutchM, 1, speed, atoi(sd_buff) );
        if(sd_buff[5]=='d') MotorRunCaclibration( &ClutchM, 0, speed, atoi(sd_buff) );
        if(sd_buff[5]=='y') MotorRunCaclibration( &BreakM, 1, speed, atoi(sd_buff) );
        if(sd_buff[5]=='h') MotorRunCaclibration( &BreakM, 0, speed, atoi(sd_buff) );

        if(sd_buff[5]=='z') ClutchM.max_position = atoi(sd_buff);
        if(sd_buff[5]=='x') BreakM.max_position = atoi(sd_buff);


        //*****PID CONTROL*******//

        if(sd_buff[5]=='p') pidCtx.kp = atoi(sd_buff)/100.0;
        if(sd_buff[5]=='i') pidCtx.ki = atoi(sd_buff)/1000.0;
        if(sd_buff[5]=='v') { control_start = 0; val = atoi(sd_buff); }
        if(sd_buff[0]=='s') { control_start = 0; val = 55; }
        if(sd_buff[5]=='b') es = atoi(sd_buff);
        if(sd_buff[0]=='a') control_start = 1;
        if(sd_buff[5]=='l') pidCtx.integrLimit = atoi(sd_buff);
        if(sd_buff[5]=='k') CSErrorDeadzoneHalfwidth = atoi(sd_buff);
        
        extDacSetValue(( uint8_t)(val*0.55),val);

        chprintf( (BaseSequentialStream *)&SD3, "State: %d State: %d  Mode: %d  Position1: %d  Position2: %d Max1: %d Max2: %d Track1: %d Track2: %d \r\n",ClutchM.state,BreakM.state , ClutchM.mode, ClutchM.position ,BreakM.position ,ClutchM.max_position ,BreakM.max_position , ClutchM.tracked_position,BreakM.tracked_position);
        //chprintf( (BaseSequentialStream *)&SD7,"{%d,%d,%d,%d}",ClutchM.position,BreakM.position,speed,speed);
         for (int i = 0; i < 9; i++)
        {
          sd_buff[i]='?';
          sd_buff2[i]='?';
        }
        chThdSleepMilliseconds( 500 );
    }
}

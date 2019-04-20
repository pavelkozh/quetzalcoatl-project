#include <tests.h>
#include <lld_can.h>
#include <lld_ext_dac.h>
#include <controllers.h>
#include <lld_control.h>
#include <chprintf.h>


#define CSErrorDeadzoneHalfwidth 1


void RisingMTD1Callback(PWMDriver *pwmd);

void fallingMTD1Callback(PWMDriver *pwmd);



//float       EngineSpeed = 770.0;
uint8_t     val = 55;  //50 = 1V
float       es = 770.0;
bool        control_start = 0;

extern  gazelParam gazel;

static PIDControllerContext_t  pidCtx = {
    .kp   = 1,
    .ki   = 0.0,
    .kd   = 0,
    .integrLimit  = 4000
};

MotorDriver MTD1 = {
    .pwmd            =   &PWMD3,
    .dir_line        =   PAL_LINE(GPIOB, 8),
    .rising_edge_cb  =   RisingMTD1Callback,
    .falling_edge_cb =   fallingMTD1Callback,
    .max_position    =   8000
};



void RisingMTD1Callback(PWMDriver *pwmd){

    (void) pwmd;
    risingEdgeCb(&MTD1);
    palToggleLine(PAL_LINE(GPIOB,7));

}
void fallingMTD1Callback(PWMDriver *pwmd){

    (void) pwmd;
    fallingEdgeCb(&MTD1);
    palToggleLine(PAL_LINE(GPIOB,14));

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

    engine_speed_rpm = map(engine_speed_rpm,0,5000,0,1000);

    //engine_speed_rpm  = CLIP_VALUE( engine_speed_rpm, 0, 100 );

    int16_t current_engine_speed = map(gazel.EngineSpeed,0,5000,0,1000);

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
    controlValue = map(controlValue,0,1000,75,255);

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


    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );   //Led
    can_init();
    extDacInit();
    chThdCreateStatic(pid_wa, sizeof(pid_wa), NORMALPRIO + 10, pid, NULL);

      /*MotoR driver Setting */
  palSetLineMode( PAL_LINE( GPIOC, 6),  PAL_MODE_ALTERNATE(2) );
  palSetLineMode( MTD1.dir_line, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLine(MTD1.dir_line);
  MotorlldControlInit( &MTD1 );


    uint32_t CPSpeed = 5000;
    uint32_t steps = 4000;
    uint16_t speed = 4000;

    static char  sd_buff[10] ;
    while(1) {

      sdReadTimeout( &SD3, sd_buff, 6, TIME_IMMEDIATE   );

      if(sd_buff[5]=='q') {
        speed = atoi(sd_buff);
        MotorSetSpeed( &MTD1, speed); 
      }



      if(sd_buff[0]=='r') MotorRunContinuous( &MTD1, 1, speed);
      if(sd_buff[0]=='l') MotorRunContinuous( &MTD1, 0, speed);
      if(sd_buff[0]=='t') MotorRunTracking( &MTD1, speed);
      if(sd_buff[5]=='o') MTD1.tracked_position = atoi(sd_buff);
      if(sd_buff[0]=='c') MotorStop( &MTD1 );
      if(sd_buff[0]=='m') MTD1.max_position = MTD1.position;
      if(sd_buff[0]=='e') {if(MTD1.state == MOTOR_MOVING) MotorStop( &MTD1 ); MotorResetPotision( &MTD1);}
      if(sd_buff[5]=='h') MotorRunCaclibration( &MTD1, 1, speed, atoi(sd_buff) );
      if(sd_buff[5]=='j') MotorRunCaclibration( &MTD1, 0, speed, atoi(sd_buff) );
      if(sd_buff[5]=='z') MTD1.max_position = atoi(sd_buff);

        if(sd_buff[5]=='p') pidCtx.kp = atoi(sd_buff)/100.0;
        if(sd_buff[5]=='i') pidCtx.ki = atoi(sd_buff)/100.0;
        if(sd_buff[5]=='d') pidCtx.kd = atoi(sd_buff)/100.0;
        if(sd_buff[5]=='v') { control_start = 0; val = atoi(sd_buff); }
        if(sd_buff[0]=='s') { control_start = 0; val = 55; }
        if(sd_buff[5]=='b') es = atoi(sd_buff);
        if(sd_buff[0]=='a') control_start = 1;
        
        extDacSetValue(( uint8_t)(val*0.55),val);
      for (int i = 0; i < 9; i++)
      {
        sd_buff[i]='?';
      }

        chprintf( (BaseSequentialStream *)&SD3, "A: %d Pedal: %.1f ESpeed: %.02f  VSeed: %.2f _________es: %d Kp: %.02f ki: %.02f Kd:%.02f  ISum: %.3f ___________ State: %d  Mode: %d  Position: %d Max: %d Track:\r\n", val, gazel.AcceleratorPedalPosition, gazel.EngineSpeed, gazel.Speed ,es, pidCtx.kp, pidCtx.ki, pidCtx.kd,pidCtx.integrSum, MTD1.state , MTD1.mode, MTD1.position ,MTD1.max_position , MTD1.tracked_position);
        chThdSleepMilliseconds( 500 );
    }
}

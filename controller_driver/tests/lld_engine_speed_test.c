#include <tests.h>
#include <lld_can.h>
#include <lld_ext_dac.h>
#include <controllers.h>
#include <lld_control.h>
#include <chprintf.h>


#define CSErrorDeadzoneHalfwidth 1


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
            val = 55;
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
    drivelldControlInit();


    uint32_t CPSpeed = 5000;
    uint32_t steps = 4000;
    driveSetSpeed(CPSpeed);

    static char  sd_buff[10] ;
    while(1) {

        sdReadTimeout( &SD3, sd_buff, 5, TIME_IMMEDIATE);

        if(sd_buff[4]=='r') { steps = 4000 * atoi(sd_buff); driveSetDirection(1); driveRun(NULL,steps,CPSpeed);}
        if(sd_buff[4]=='l') { steps = 4000 * atoi(sd_buff); driveSetDirection(0); driveRun(NULL,steps,CPSpeed);}
        if(sd_buff[4]=='z') { CPSpeed = atoi(sd_buff); driveSetSpeed(CPSpeed);}
        if(sd_buff[4]=='c')  driveStop();
        if(sd_buff[4]==']')  driveSetDirection(1);
        if(sd_buff[4]=='[')  driveSetDirection(0);

        if(sd_buff[4]=='p') pidCtx.kp = atoi(sd_buff)/100.0;
        if(sd_buff[4]=='i') pidCtx.ki = atoi(sd_buff)/100.0;
        if(sd_buff[4]=='d') pidCtx.kd = atoi(sd_buff)/100.0;
        if(sd_buff[4]=='m') { control_start = 0; val = atoi(sd_buff); }
        if(sd_buff[4]=='e') es = atoi(sd_buff);
        //if(sd_buff[4]=='g') EngineSpeed = atoi(sd_buff);
        if(sd_buff[0]=='s') control_start = 0;
        if(sd_buff[0]=='a') control_start = 1;
        
        extDacSetValue(( uint8_t)(val*0.55),val);


        chprintf( (BaseSequentialStream *)&SD3, "A: %d Pedal: %.1f ESpeed: %.02f  VSeed: %.2f _________es: %d Kp: %.02f ki: %.02f Kd:%.02f  ISum: %.3f ______CPSpeed: %d \r\n", val, gazel.AcceleratorPedalPosition, gazel.EngineSpeed, gazel.Speed ,es, pidCtx.kp, pidCtx.ki, pidCtx.kd,pidCtx.integrSum, CPSpeed);
        chThdSleepMilliseconds( 500 );
    }
}

#include <tests.h>
// #include <lld_can.h>
#include <lld_ext_dac.h>
#include <controllers.h>
#include <lld_control.h>
//#include <fuzzy_logic.h>
// #include <lld_px4flow.h>
#include <feedback.h>
//#include <low_speed_control.h>
#include <chprintf.h>



void RisingEdgeClutchMCallback(PWMDriver *pwmd);

void fallingEdgeClutchMCallback(PWMDriver *pwmd);

void RisingEdgeBreakMCallback(PWMDriver *pwmd);

void fallingEdgeBreakMCallback(PWMDriver *pwmd);

static uint8_t     val = 55;  //50 = 1V
static float       es = 770.0;
static bool        control_start = 0;
static uint8_t CSErrorDeadzoneHalfwidth = 0;

static PIDControllerContext_t  pidCtx = {
    .kp   = 0,
    .ki   = 0.01,
    .kd   = 0,
    .integrLimit  = 8000
};


extern  gazelParam gazel;

MotorDriver ClutchM = {
    .pwmd            =   &PWMD3,
    .dir_line        =   PAL_LINE(GPIOB, 8),
    .rising_edge_cb  =   RisingEdgeClutchMCallback,
    .falling_edge_cb =   fallingEdgeClutchMCallback,
    .max_position    =   100000,
    .min_position    =  0
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
    .max_position    =   30000,
    .min_position    =   0
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


static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};



void TestFLRouting ( void )
{
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX


    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );   //Led
    can_init();
    extDacInit();
    //chThdCreateStatic(pid_wa, sizeof(pid_wa), NORMALPRIO + 5, pid, NULL);
    //lowSpeedControlInti()
    feedbackInit();

      /*MotoR driver Setting */
    palSetLineMode( PAL_LINE( GPIOC, 6),  PAL_MODE_ALTERNATE(2) );
    palSetLineMode( ClutchM.dir_line, PAL_MODE_OUTPUT_PUSHPULL);

    MotorlldControlInit( &ClutchM );

    palSetLineMode( PAL_LINE( GPIOD, 12),  PAL_MODE_ALTERNATE(2) );
    palSetLineMode( BreakM.dir_line, PAL_MODE_OUTPUT_PUSHPULL);
    MotorlldControlInit( &BreakM );


    uint32_t CPSpeed = 5000;
    uint32_t steps = 4000;
    uint16_t speed = 6000;
    int32_t filter_px4flow = 0;


    double res_buff2[2] = {0.0,0.0};
    double speed222 = GazleGetSpeed();

    static char  sd_buff[10] = {'?','?','?','?','?','?','?','?','?','?'} ;

    while(1) {
//*************
        sdReadTimeout( &SD3, sd_buff, 10, TIME_IMMEDIATE   );


        if(sd_buff[5]=='n') {
          speed = atoi(sd_buff);
          MotorSetSpeed( &ClutchM, speed); 
          MotorSetSpeed( &BreakM, speed); 
        }

        // //*****MOTOR CONTROL*******//

        // if(sd_buff[0]=='r') {fl_start_flag = 0;MotorRunContinuous( &ClutchM, 1, speed);}
        // if(sd_buff[0]=='f') {fl_start_flag = 0;MotorRunContinuous( &ClutchM, 0, speed);}
        // if(sd_buff[0]=='t') {fl_start_flag = 0;MotorRunContinuous( &BreakM, 1, speed);}
        // if(sd_buff[0]=='g') {fl_start_flag = 0;MotorRunContinuous( &BreakM, 0, speed);}

    if(sd_buff[0]=='r'){
        MotorRunContinuous( &ClutchM, 0, speed);
        chThdSleepMilliseconds( 500 );
        MotorStop(&ClutchM);
        chThdSleepMilliseconds( 500 );
        MotorRunContinuous( &ClutchM, 1, speed);
    }
    if(sd_buff[0]=='f'){
        MotorRunTracking( &ClutchM, speed);
        ClutchM.tracked_position = 25000;
        chThdSleepMilliseconds( 500 );
        ClutchM.tracked_position = 50000;
        chThdSleepMilliseconds( 500 );
        ClutchM.tracked_position = 75000;
        chThdSleepMilliseconds( 500 );
        ClutchM.tracked_position = 0;
    }

    if(sd_buff[0]=='t'){
        MotorRunCaclibration( &ClutchM, 0, speed, 10000 );
        chThdSleepMilliseconds( 500 );
        MotorRunCaclibration( &ClutchM, 1, speed, 10000 );
    }
        // if(sd_buff[0]=='u') { MotorRunTracking( &ClutchM, speed); MotorRunTracking( &BreakM, speed);}

        // if(sd_buff[5]=='q') ClutchM.tracked_position = atoi(sd_buff);
        // if(sd_buff[5]=='w') BreakM.tracked_position = atoi(sd_buff);

        // if(sd_buff[0]=='c') { MotorStop( &ClutchM );  MotorStop( &BreakM ); }


        // if(sd_buff[5]=='e') MotorRunCaclibration( &ClutchM, 1, speed, atoi(sd_buff) );
        // if(sd_buff[5]=='d') MotorRunCaclibration( &ClutchM, 0, speed, atoi(sd_buff) );
        // if(sd_buff[5]=='y') MotorRunCaclibration( &BreakM, 1, speed, atoi(sd_buff) );
        // if(sd_buff[5]=='h') MotorRunCaclibration( &BreakM, 0, speed, atoi(sd_buff) );

        // if(sd_buff[5]=='z') ClutchM.max_position = atoi(sd_buff);
        // if(sd_buff[5]=='x') BreakM.max_position = atoi(sd_buff);


        // //*****PID CONTROL*******//

        // if(sd_buff[5]=='p') pidCtx.kp = atoi(sd_buff)/100.0;
        // if(sd_buff[5]=='i') pidCtx.ki = atoi(sd_buff)/1000.0;
        // if(sd_buff[5]=='v') { control_start = 0; val = atoi(sd_buff); }
        // if(sd_buff[0]=='s') { control_start = 0; val = 55; }
        // if(sd_buff[5]=='b') es = atoi(sd_buff);
        // if(sd_buff[0]=='a') control_start = 1;
        // if(sd_buff[5]=='l') pidCtx.integrLimit = atoi(sd_buff);
        // if(sd_buff[5]=='k') CSErrorDeadzoneHalfwidth = atoi(sd_buff);
        // extDacSetValue(( uint8_t)(val*0.55),val);

        //*****Fuzzy CONTROL*******//

        //  if(sd_buff[0]=='q') vs  = atoi(&sd_buff[1])/100.0; 
        // if(sd_buff[0]=='w') max_clutch_pos = atoi(&sd_buff[1]); 
        // if(sd_buff[0]=='e') min_clutch_pos = atoi(&sd_buff[1]);
        // if(sd_buff[0]=='d') max_break_pos = atoi(&sd_buff[1]);
        // if(sd_buff[0]=='s') min_break_pos = atoi(&sd_buff[1]);
        // if(sd_buff[0]=='z') {
        //     dVSpeed_t = atoi( &sd_buff[1] ) /100.0-0.3;
        //     vs = vs - dVSpeed_t;
        //     calculateFLReg(-vs, dVSpeed_t, &res_buff2);
        // }
        // if(sd_buff[0]=='z') VSpeed = atoi(&sd_buff[1])/100.0;
        // if(sd_buff[0]=='v') dVSpeed = atoi(&sd_buff[1])/100.0;
        // if(sd_buff[0]=='x'){
        //                     fl_start_flag = 1;
        //                 } 
        // if(sd_buff[0]=='c') fl_start_flag = 0;
        // if(ground_distance()>1000)
        //  filter_px4flow = flow_comp_m_x()*0.0144;
        // chprintf( (BaseSequentialStream *)&SD3, "State: %d State: %d  Mode: %d  Position1: %d  Position2: %d Max1: %d Max2: %d Track1: %d Track2: %d \r\n",ClutchM.state,BreakM.state , ClutchM.mode, ClutchM.position ,BreakM.position ,ClutchM.max_position ,BreakM.max_position , ClutchM.tracked_position,BreakM.tracked_position);
        //chprintf( (BaseSequentialStream *)&SD3,"fl_start_flag: %d vs: %.2f, GazSp: %.2f, dGazSp: %.3f Csp: %.2f, Bsp: %.2f__________ min_CSp: %d, max_CSp: %d, min_Bsp: %d, max_Bsp: %d \n\r",fl_start_flag, VSpeed_e, gazel.Speed,dErr, res_buff[0], res_buff[1], min_clutch_speed, max_clutch_speed, min_break_speed, max_break_speed); 

        // chprintf( (BaseSequentialStream *)&SD3, "State: %d State: %d  Mode: %d  Position1: %d  Position2: %d Max1: %d Max2: %d Track1: %d Track2: %d \r\n",ClutchM.state,BreakM.state , ClutchM.mode, ClutchM.position ,BreakM.position ,ClutchM.max_position ,BreakM.max_position , ClutchM.tracked_position,BreakM.tracked_position);
        //chprintf( (BaseSequentialStream *)&SD3,"vs:\t%.2f\tESp:\t%.2f\tPx4flow:\t%.02f\tdGazSp:\t%.3f\tCsp:\t%.2f\tClutchM:\t%d\tBreakM:\t%d\t engS:\t%.02f\tSdil:%0.2f\n\r",vs, VSpeed_e,(gazel.Speed_px4flow/20.0)*0.0036,dErr, res_buff[0],ClutchM.position,BreakM.position,gazel. EngineSpeed,VSpeed_e_filter); 

        //chprintf( (BaseSequentialStream *)&SD3,"P %.02f GazSp: %.2f\t x: %.02f\t gnd: %.02f\n\r", flow_comp_m_x()*0.0036,ground_distance()/1000.0);
        chprintf((BaseSequentialStream *)&SD3," VALUEL: %.2f \n\r",GazleGetSpeed());
        palTogglePad( GPIOB, 0);
        for (int i = 0; i < 9; i++)
        {
          sd_buff[i]='?';
        }
        //chprintf( (BaseSequentialStream *)&SD3,"val: %.02f ,ln: %.02f,mn: %.02f,sn: %.02f,no: %.02f,sp: %.02f,mp: %.02f,lp: %.02f \n\r", testvar_out, fuzzy_speed.output_val.ln,fuzzy_speed.output_val.mn,fuzzy_speed.output_val.sn,fuzzy_speed.output_val.no,fuzzy_speed.output_val.sp,fuzzy_speed.output_val.mp,fuzzy_speed.output_val.lp);
        chThdSleepMilliseconds( 500 );
    }
}

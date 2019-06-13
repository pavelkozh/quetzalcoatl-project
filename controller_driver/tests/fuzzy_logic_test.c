#include <tests.h>
// #include <lld_can.h>
#include <lld_ext_dac.h>
#include <controllers.h>
#include <lld_control.h>
#include <fuzzy_logic.h>
// #include <lld_px4flow.h>
#include <feedback.h>
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


uint32_t map2(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


uint32_t engineSpeedControl( uint32_t engine_speed_rpm )
{

    engine_speed_rpm = map2(engine_speed_rpm,0,5000,0,5000);

    //engine_speed_rpm  = CLIP_VALUE( engine_speed_rpm, 0, 100 );

    int16_t current_engine_speed = map2(GazleGetSpeed(),0,5000,0,5000);

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
    controlValue = CLIP_VALUE(controlValue,0,5000);// 55 -> 215; 107 -> 420
    controlValue = map2(controlValue,0,5000,77,255);

    return controlValue;

}
static THD_WORKING_AREA(pid_wa, 128);
static THD_FUNCTION(pid, arg) {
    
    (void)arg;

    PIDControlInit( &pidCtx );
    palSetPad(GPIOB,0);
    
    while(1){
        if (control_start ){
            val = engineSpeedControl(( uint32_t ) es);
        }
        else{
                val = 58;
                pidCtx.err        = 0;
                pidCtx.prevErr    = 0;
                pidCtx.integrSum  = 0;
        }
        
        extDacSetValue(( uint8_t)(val*0.55),val);  
        chThdSleepMilliseconds( 20 );
    }
}

uint32_t map(double x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static double res_buff[2] = {0.0,0.0};
static double VSpeed_e_prev = 0.0;
static double VSpeed_e = 0.0;
static double VSpeed_e_buf[20] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
static double VSpeed_e_sum = 0.0;
static double VSpeed_e_filter = 0.0;
static double VSpeed = 0.0;
static double dErr= 0.0;
static uint8_t  E_cnt = 0;
static double  vs = 0;
static bool fl_start_flag = 0;
static target_zone_flag = 0;
static uint8_t pedal_calibration_start_fag = 0;

static uint32_t max_clutch_pos     = 75000;
static uint32_t min_clutch_pos     = 50000;
static uint16_t max_break_pos     = 2000;
static uint16_t min_break_pos      = 0;

static uint32_t max_clutch_speed = 4000;
static uint32_t min_clutch_speed = 20000;

static THD_WORKING_AREA(fl_wa, 256);
static THD_FUNCTION(fl, arg) {
    (void)arg;
    /*Clutch and Breake speed*/
    /*Fuzzy logic controller*/
    bool Clutch_old = 0;
    bool Break_old = 0;
    fuzzylogicInit();

    while(1){

        // if(pedal_calibration_start_fag >= 1){
        //     fl_start_flag = 0;
        //     if((gazel.ClutchSwitch==1) && (pedal_calibration_start_fag == 1))
        //         MotorRunContinuous( &ClutchM, 1, 1000 );
        //     else
        //         MotorRunContinuous( &ClutchM, 0, 1000 );
        //     if((gazel.BrakeSwitch==1) && (pedal_calibration_start_fag == 1))
        //         MotorRunContinuous( &BreakM, 1, 1000 );
        //     else
        //         MotorRunContinuous( &BreakM, 0, 1000 );

        //     pedal_calibration_start_fag = 2;
        //     if((gazel.ClutchSwitch != Clutch_old) && (pedal_calibration_start_fag == 2)){
        //         MotorStop( &ClutchM );
        //         pedal_calibration_start_fag = 0;
        //     }
        //     if((gazel.BrakeSwitch != Break_old) && (pedal_calibration_start_fag == 2)){
        //         MotorStop( &BreakM );
        //         pedal_calibration_start_fag = 0;
        //     }
        // }else{
        //    Clutch_old = gazel.ClutchSwitch;
        //    Break_old = gazel.BrakeSwitch;
        // }
        if(fl_start_flag == 1){
                // MotorRunTracking( &ClutchM, 3000); MotorRunTracking( &BreakM, 3000);
                // if(vs==0){
                //     MotorRunContinuous( &ClutchM, 0, 5000 ); //100 + res_buff becouse max_clutch_speed < min_clutch_speed and res_buff < 0
                //     if(ClutchM.State == 0)
                //         MotorRunContinuous( &BreakM, 0, 5000 );
                // }else{

                if(target_zone_flag == 0 ){

                    if(ClutchM.position > 72000)
                        MotorRunContinuous( &ClutchM, 1, 1000 );
                    else
                        MotorRunContinuous( &ClutchM, 1, 35000 );

                    if( ((gazel.Speed_px4flow/20.0)*0.0036) >=0.2){
                        max_clutch_pos = ClutchM.position + 5000;
                        min_clutch_pos = ClutchM.position -40000;
                        target_zone_flag = 1;
                    }
                }else{

                    VSpeed_e= vs - ( 0 / 20.0)*0.0036 ;

                    // VSpeed_e_sum -= VSpeed_e_buf[E_cnt];
                    // VSpeed_e_buf[E_cnt] = VSpeed_e;
                    // VSpeed_e_sum += VSpeed_e_buf[E_cnt];
                    // E_cnt = E_cnt >= 19 ? 0 : E_cnt+1;

                    // VSpeed_e_filter = (VSpeed_e_sum/20.0);
                    dErr =  VSpeed_e - VSpeed_e_prev;

                    //calculate fl
                    calculateFLReg(VSpeed_e, dErr, &res_buff);
                    VSpeed_e_prev = VSpeed_e;

                    // MotorRunTracking( &ClutchM, 1500);
                    // MotorRunTracking( &BreakM, 3000);
                    // if(res_buff[0]<0){
                    //     BreakM.tracked_position = 0;
                    //     ClutchM.tracked_position = map(res_buff[0]+100,0,100,min_clutch_pos,max_clutch_pos);
                    // }
                    // if (res_buff[0]>0){
                    //     ClutchM.tracked_position = max_clutch_pos;
                    //     BreakM.tracked_position = map(res_buff[0],0,100,min_break_pos,max_break_pos);
                    // }

                if( (res_buff[0]<0) && (ClutchM.position >= min_clutch_pos)){
                    uint32_t cl_sp =  map(100 + res_buff[0], 0,100, max_clutch_speed, min_clutch_speed);
                    MotorRunContinuous( &ClutchM, 1, cl_sp ); //100 + res_buff becouse max_clutch_speed < min_clutch_speed and res_buff < 0
                }else
                    if((res_buff[0]>0) && (ClutchM.position <= max_clutch_pos)){
                        uint32_t cl_sp =   map(100 - res_buff[0], 0,100, max_clutch_speed, min_clutch_speed);
                        MotorRunContinuous( &ClutchM, 0, cl_sp); //100 + res_buff becouse max_clutch_speed < min_clutch_speed and res_buff < 0
                    }else 
                        MotorStop(&ClutchM);
                }


                // if(res_buff[1]<0)
                //     MotorRunContinuous( &BreakM, 1, map(100 + res_buff[1], 0,100, max_break_speed, min_break_speed) ); //100 + res_buff becouse max_breake_speed < min_break_speed and res_buff < 0
                // else if(res_buff[1]>0)
                //     MotorRunContinuous( &BreakM, 0, map(100 - res_buff[1], 0,100, max_break_speed, min_break_speed) ); //100 + res_buff becouse max_breake_speed < min_break_speed and res_buff < 0
                // else 
                //     MotorStop(&BreakM);
                // }
            // }
        // }else{
            //MotorRunContinuous( &ClutchM, 0, 600);
        }else{
            target_zone_flag = 0;
        }
            chThdSleepMilliseconds( 30 );
        
    }
}

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
    chThdCreateStatic(fl_wa, sizeof(fl_wa), NORMALPRIO + 6, fl, NULL);
     //Fuzzy logic controller
     fuzzylogicInit();
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

         if(sd_buff[0]=='q') vs  = atoi(&sd_buff[1])/100.0; 
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
        if(sd_buff[0]=='x'){
                            fl_start_flag = 1;
                        } 
        if(sd_buff[0]=='c') fl_start_flag = 0;
        // if(ground_distance()>1000)
        //  filter_px4flow = flow_comp_m_x()*0.0144;
        // chprintf( (BaseSequentialStream *)&SD3, "State: %d State: %d  Mode: %d  Position1: %d  Position2: %d Max1: %d Max2: %d Track1: %d Track2: %d \r\n",ClutchM.state,BreakM.state , ClutchM.mode, ClutchM.position ,BreakM.position ,ClutchM.max_position ,BreakM.max_position , ClutchM.tracked_position,BreakM.tracked_position);
        //chprintf( (BaseSequentialStream *)&SD3,"fl_start_flag: %d vs: %.2f, GazSp: %.2f, dGazSp: %.3f Csp: %.2f, Bsp: %.2f__________ min_CSp: %d, max_CSp: %d, min_Bsp: %d, max_Bsp: %d \n\r",fl_start_flag, VSpeed_e, gazel.Speed,dErr, res_buff[0], res_buff[1], min_clutch_speed, max_clutch_speed, min_break_speed, max_break_speed); 

        // chprintf( (BaseSequentialStream *)&SD3, "State: %d State: %d  Mode: %d  Position1: %d  Position2: %d Max1: %d Max2: %d Track1: %d Track2: %d \r\n",ClutchM.state,BreakM.state , ClutchM.mode, ClutchM.position ,BreakM.position ,ClutchM.max_position ,BreakM.max_position , ClutchM.tracked_position,BreakM.tracked_position);
        //chprintf( (BaseSequentialStream *)&SD3,"vs:\t%.2f\tESp:\t%.2f\tPx4flow:\t%.02f\tdGazSp:\t%.3f\tCsp:\t%.2f\tClutchM:\t%d\tBreakM:\t%d\t engS:\t%.02f\tSdil:%0.2f\n\r",vs, VSpeed_e,(gazel.Speed_px4flow/20.0)*0.0036,dErr, res_buff[0],ClutchM.position,BreakM.position,gazel. EngineSpeed,VSpeed_e_filter); 

        chprintf( (BaseSequentialStream *)&SD3,"P %.02f GazSp: %.2f\t x: %.02f\t gnd: %.02f\n\r", flow_comp_m_x()*0.0036,ground_distance()/1000.0);
        for (int i = 0; i < 9; i++)
        {
          sd_buff[i]='?';
        }
        //chprintf( (BaseSequentialStream *)&SD3,"val: %.02f ,ln: %.02f,mn: %.02f,sn: %.02f,no: %.02f,sp: %.02f,mp: %.02f,lp: %.02f \n\r", testvar_out, fuzzy_speed.output_val.ln,fuzzy_speed.output_val.mn,fuzzy_speed.output_val.sn,fuzzy_speed.output_val.no,fuzzy_speed.output_val.sp,fuzzy_speed.output_val.mp,fuzzy_speed.output_val.lp);
        chThdSleepMilliseconds( 50 );
    }
}

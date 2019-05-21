#include <tests.h>
#include <lld_can.h>
#include <lld_ext_dac.h>
#include <controllers.h>
#include <lld_control.h>
#include <fuzzy_logic.h>
#include <chprintf.h>





void RisingEdgeClutchMCallback(PWMDriver *pwmd);

void fallingEdgeClutchMCallback(PWMDriver *pwmd);

void RisingEdgeBreakMCallback(PWMDriver *pwmd);

void fallingEdgeBreakMCallback(PWMDriver *pwmd);




extern  gazelParam gazel;

MotorDriver ClutchM = {
    .pwmd            =   &PWMD3,
    .dir_line        =   PAL_LINE(GPIOB, 8),
    .rising_edge_cb  =   RisingEdgeClutchMCallback,
    .falling_edge_cb =   fallingEdgeClutchMCallback,
    .max_position    =   1000,
    .min_position    =  -1000
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

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

void TestFLRouting ( void )
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
    // chThdCreateStatic(pid_wa, sizeof(pid_wa), NORMALPRIO + 10, pid, NULL);

      /*MotoR driver Setting */
    palSetLineMode( PAL_LINE( GPIOC, 6),  PAL_MODE_ALTERNATE(2) );
    palSetLineMode( ClutchM.dir_line, PAL_MODE_OUTPUT_PUSHPULL);

    MotorlldControlInit( &ClutchM );

    palSetLineMode( PAL_LINE( GPIOD, 12),  PAL_MODE_ALTERNATE(2) );
    palSetLineMode( BreakM.dir_line, PAL_MODE_OUTPUT_PUSHPULL);
    MotorlldControlInit( &BreakM );

    /*Fuzzy logic controller*/
    fuzzylogicInit();

    uint32_t CPSpeed = 5000;
    uint32_t steps = 4000;
    uint16_t speed = 4000;


    static char  sd_buff[10] = {'?','?','?','?','?','?','?','?','?','?'} ;
    static double res_buff[2] = {1.0,1.0};
    float  Vs_e  = -5.0;
    float VSpeed_prev = -2.5;
    float  dVs_e = -2.5;
    uint32_t  Cl_pos  = 38300;
    uint32_t  B_pos   = 13100;

    while(1) {

        sdReadTimeout( &SD3, sd_buff, 6, TIME_IMMEDIATE   );
        //sdReadTimeout( &SD7, sd_buff2, 6, TIME_IMMEDIATE   );
        // //sd_buff2[9] = 0;

        // if(sd_buff2[5]=='q') ClutchM.tracked_position = atoi(sd_buff);
        // if(sd_buff2[5]=='w') BreakM.tracked_position = atoi(sd_buff);

        // if(sd_buff[5]=='n') {
        //   speed = atoi(sd_buff);
        //   MotorSetSpeed( &ClutchM, speed); 
        // }

        // //*****MOTOR CONTROL*******//

        // if(sd_buff[0]=='r') MotorRunContinuous( &ClutchM, 1, speed);
        // if(sd_buff[0]=='f') MotorRunContinuous( &ClutchM, 0, speed);
        // if(sd_buff[0]=='t') MotorRunContinuous( &BreakM, 1, speed);
        // if(sd_buff[0]=='g') MotorRunContinuous( &BreakM, 0, speed);

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

        if(sd_buff[5]=='q') Vs_e  = atoi(sd_buff)/100.0-5; 
        if(sd_buff[0]=='w') { dVs_e = Vs_e-VSpeed_prev; VSpeed_prev = Vs_e;}
        if(sd_buff[5]=='e') Cl_pos  = atoi(sd_buff) ;
        if(sd_buff[5]=='r') B_pos   = atoi(sd_buff) ;
        if(sd_buff[0]=='s') calculateFLReg(Vs_e,dVs_e,Cl_pos,B_pos,&res_buff);

        // chprintf( (BaseSequentialStream *)&SD3, "State: %d State: %d  Mode: %d  Position1: %d  Position2: %d Max1: %d Max2: %d Track1: %d Track2: %d \r\n",ClutchM.state,BreakM.state , ClutchM.mode, ClutchM.position ,BreakM.position ,ClutchM.max_position ,BreakM.max_position , ClutchM.tracked_position,BreakM.tracked_position);
        chprintf( (BaseSequentialStream *)&SD3,"VSpeed_err: %.2f, dVSpeed_err: %.2f, Clutch_pos: %d, Break_pos: %d, Cs: %.2f, Bs: %.2f \n\r",Vs_e,dVs_e,Cl_pos,B_pos,res_buff[0],res_buff[1]);
         for (int i = 0; i < 9; i++)
        {
          sd_buff[i]='?';
        }
        //chprintf( (BaseSequentialStream *)&SD3,"val: %.02f ,ln: %.02f,mn: %.02f,sn: %.02f,no: %.02f,sp: %.02f,mp: %.02f,lp: %.02f \n\r", testvar_out, fuzzy_speed.output_val.ln,fuzzy_speed.output_val.mn,fuzzy_speed.output_val.sn,fuzzy_speed.output_val.no,fuzzy_speed.output_val.sp,fuzzy_speed.output_val.mp,fuzzy_speed.output_val.lp);
        chThdSleepMilliseconds( 500 );
    }
}

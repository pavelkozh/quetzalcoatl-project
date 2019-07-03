#include <tests.h>
// #include <lld_can.h>
#include <lld_ext_dac.h>
#include <speed.h>
#include <lld_control.h>
//#include <fuzzy_logic.h>
// #include <lld_px4flow.h>
#include <feedback.h>
#include <low_speed_control.h>
#include <chprintf.h>
#include <pedals.h>


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
    lowSpeedControlIntit();
    feedbackInit();
    pedalsInit();
    speedPIDInit();


    uint32_t CPSpeed = 5000;
    uint32_t steps = 4000;
    uint16_t speed = 6000;
    int32_t filter_px4flow = 0;
    float vs = 0;

    double res_buff2[2] = {0.0,0.0};
    double speed222 = gazelGetSpeed();

    static char  sd_buff[10] = {'?','?','?','?','?','?','?','?','?','?'} ;

    while(1) {

        sdReadTimeout( &SD3, sd_buff, 10, TIME_IMMEDIATE   );


        if(sd_buff[5]=='n') {
            speed = atoi(sd_buff);
            pedalsClutchStop();
            pedalsBrakeStop();
        }

         //*****MOTOR CONTROL*******//

        if(sd_buff[0]=='r') {lowSpeedStop(); pedalsClutchRelease( speed );}
        if(sd_buff[0]=='f') {lowSpeedStop(); pedalsClutchPress( speed );}
        if(sd_buff[0]=='t') {lowSpeedStop(); pedalsBrakeRelease( speed );}
        if(sd_buff[0]=='g') {lowSpeedStop(); pedalsBrakePress( speed );}

         if(sd_buff[0]=='c') { pedalsClutchStop(); pedalsBrakeStop();}

        //*****Fuzzy CONTROL*******//

        if(sd_buff[0]=='q') { vs = (float) (atoi(&sd_buff[1])/100.0); lowSpeedSetReference( vs ); };
        if(sd_buff[0]=='x') lowSpeedStart();
        if(sd_buff[0]=='c') lowSpeedStop();

        //chprintf( (BaseSequentialStream *)&SD3,"vs:\t%.2f\tESp:\t%.2f\tPx4flow:\t%.02f\tdGazSp:\t%.3f\tCsp:\tClutchM:\t%d\tBreakM:\t%d\t engS:\t%.02f\n\r",vs, lowSpeedGetErr(),gazelGetSpeed(),lowSpeedGetdErr(),pedalsClutchGetPosition(),pedalsBrakeGetPosition(),gazelGetEngineSpeed());
        chprintf( (BaseSequentialStream *)&SD3,"vs: %.02f E: %.02f Speed: %.02f dE: %.02f ClutchM: %d BreakM: %d EngSpeed: %.02f \n\r",vs,lowSpeedGetErr(),gazelGetSpeed(),lowSpeedGetdErr(),pedalsClutchGetPosition(),pedalsBrakeGetPosition(),gazelGetEngineSpeed());
        //chprintf( (BaseSequentialStream *)&SD3,"P %.02f GazSp: %.2f\t x: %.02f\t gnd: %.02f\n\r", flow_comp_m_x()*0.0036,ground_distance()/1000.0);
        //chprintf((BaseSequentialStream *)&SD3," VALUEL: %.2f \n\r",gazelGetSpeed());
        palTogglePad( GPIOB, 0);
        for (int i = 0; i < 9; i++)
        {
          sd_buff[i]='?';
        }
        //chprintf( (BaseSequentialStream *)&SD3,"val: %.02f ,ln: %.02f,mn: %.02f,sn: %.02f,no: %.02f,sp: %.02f,mp: %.02f,lp: %.02f \n\r", testvar_out, fuzzy_speed.output_val.ln,fuzzy_speed.output_val.mn,fuzzy_speed.output_val.sn,fuzzy_speed.output_val.no,fuzzy_speed.output_val.sp,fuzzy_speed.output_val.mp,fuzzy_speed.output_val.lp);
        chThdSleepMilliseconds( 500 );
    }
}

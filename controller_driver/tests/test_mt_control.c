#include <tests.h>
#include <chprintf.h>

#include <speed.h>
#include <MT.h>
#include <MT_control.h>
#include <feedback.h>



static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};



void TestMTControl ( void )
{
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode ( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX

    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );   //Led


    mtControlInit ();
    //feedbackInit();
    speedInit();

    uint8_t sd_buff[10];
    /* no gear */
    int8_t current_gear = -1;

    while(1) {

        palToggleLine(LINE_LED1);

        sdReadTimeout( &SD3, sd_buff, 9, TIME_IMMEDIATE );


        if(sd_buff[1]=='g') current_gear = mtControlMannualyShiftGear( (uint8_t)atoi(sd_buff) );
        if(sd_buff[0]=='y') mtControlStart();
        if(sd_buff[0]=='h') mtControlStop();

        if(sd_buff[4]=='s') speedSetVehiclePIDReferenceValue ( (float) (atoi(sd_buff)/10.0) );
        if(sd_buff[0]=='e') speedVehicleControlStart();

        if(sd_buff[0]=='d') speedVehicleControlStop();
        if(sd_buff[0]=='z') speedEngineControlStop();



        chprintf( (BaseSequentialStream *)&SD3, " gear_num: %d gear_g_pos: %d gear_v_pos: %d v__target_pos: %d g__target_pos %d \n\r",mtControlGetCurrentGearNum(), getGorisontalPosition (), getVerticalPosition(), getVerticalTargetPosition (), getGorisontalTargetPosition());









        for (int i = 0; i < 9; i++)
        {
          sd_buff[i]='?';
        }

        chThdSleepMilliseconds( 500 );
    }
}














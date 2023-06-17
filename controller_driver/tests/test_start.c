#include <tests.h>
#include <chprintf.h>

#include <pedals.h>
#include <feedback.h>
#include <MT_control.h>
#include <emergency_stop.h>
#include <engine_ignition.h>

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};



void TestStart ( void )
{
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX

    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );   //Led


    pedalsInit();
    feedbackInit();
    mtControlInit();
    emergencyStopInit();
    engIgnitionInit();
    soundSignalInit();


    uint8_t sd_buff[10];

    while ( 1 )
    {
        palToggleLine(LINE_LED2);

        sdReadTimeout( &SD3, sd_buff, 10, TIME_IMMEDIATE   );

      //  if(sd_buff[0] == 's')  commandSet( atoi(&sd_buff[1]), 0);

        if(sd_buff[1]=='g') mtControlMannualyShiftGear( (uint8_t)atoi(sd_buff) );

        if(sd_buff[0]=='x') pedalsClutchPress( 15000 );
        if(sd_buff[0]=='f') pedalsClutchRelease( 15000 );
        if(sd_buff[0]=='z') pedalsBrakePress( 500 );
        if(sd_buff[0]=='o') pedalsBrakeRelease( 3000 );
        if(sd_buff[0]=='a') engStarterSwitchOn ();
        if(sd_buff[0]=='s') emergencyFullStop();

        if(sd_buff[0]=='j') soundSignalOn();
        if(sd_buff[0]=='k') soundSignalOff();

        chprintf( (BaseSequentialStream *)&SD3, "gear_num: %d\t Clutch_pos: %d\t Break_pos: %d\t speed %.02f\t eng_speed %.02f\t \n\r",mtControlGetCurrentGearNum(), pedalsClutchGetPosition(),pedalsBrakeGetPosition(), gazelGetSpeed(), gazelGetEngineSpeed());

        for (int i = 0; i < 9; i++)
      {
      sd_buff[i]='?';
      }

        chThdSleepMilliseconds(500);
    }


}



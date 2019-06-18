#include <tests.h>
#include <chprintf.h>

#include <pedals.h>


static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};



void TestPedals ( void )
{
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX

    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );   //Led


    pedalsInit();

    uint8_t sd_buff[10];

    while(1) {

        sdReadTimeout( &SD3, sd_buff, 9, TIME_IMMEDIATE );


        if(sd_buff[0]=='c') { pedalsClutchStop(); pedalsBrakeStop(); }

        /* Clutch test*/
        if(sd_buff[0]=='x') pedalsClutchPress( 3000 );
        if(sd_buff[0]=='f') pedalsClutchRelease( 3000 );
        if(sd_buff[6]=='e') pedalsClutchMove(atoi(sd_buff),3000);
        if(sd_buff[5]=='y') pedalsClutchCalibrate(0, 3000, atoi(sd_buff));
        if(sd_buff[5]=='q') pedalsClutchCalibrate(1, 3000, atoi(sd_buff));
        if(sd_buff[6]=='n') pedalsClutchChangeSpeed ( atoi(sd_buff) );

        chprintf( (BaseSequentialStream *)&SD3, "ClState %d\t ClMode %d\t ClStatePos %d\t ClSpeed %d\t \n\r",pedalsClutchGetState(), pedalsClutchGetMode(), pedalsClutchGetPosition(), pedalsClutchGetSpeed());
//
        /* Brake test*/
//        if(sd_buff[0]=='z') pedalsBrakePress( 3000 );
//        if(sd_buff[0]=='o') pedalsBrakeRelease( 3000 );
//        if(sd_buff[6]=='r') pedalsBrakeMove(atoi(sd_buff), 3000);
//        if(sd_buff[5]=='h') pedalsBrakeCalibrate (0, 3000, atoi(sd_buff));
//        if(sd_buff[5]=='w') pedalsBrakeCalibrate (1, 3000, atoi(sd_buff));
//        if(sd_buff[6]=='g') pedalsBrakeChangeSpeed ( atoi(sd_buff) );

//        chprintf( (BaseSequentialStream *)&SD3, "BrState %d\t BrMode %d\t BrStatePos %d\t BrSpeed %d\t \n\r",pedalsBrakeGetState(), pedalsBrakeGetMode(), pedalsBrakeGetPosition(), pedalsBrakeGetSpeed());

        /* Accelerator pedal test*/
//        if(sd_buff[4]=='u') pedalsAcceleratorControl ( atoi(sd_buff) );




        for (int i = 0; i < 9; i++)
        {
          sd_buff[i]='?';
        }

        chThdSleepMilliseconds( 500 );
    }
}














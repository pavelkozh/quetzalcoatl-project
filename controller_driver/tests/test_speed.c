#include <tests.h>
#include <chprintf.h>

#include <speed.h>
#include <feedback.h>
#include <pedals.h>



static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};



void TestSpeed ( void )
{
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX

    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );   //Led

    //palSetLine(LINE_LED2);
    speedInit();
    feedbackInit();
    pedalsInit();


    uint8_t sd_buff[10];


    while(1) {

        //palToggleLine(LINE_LED1);

        sdReadTimeout( &SD3, sd_buff, 9, TIME_IMMEDIATE );


        if(sd_buff[4]=='s') speedSetVehiclePIDReferenceValue ( (float)(atoi(sd_buff)/10.0) ) ;
        if(sd_buff[0]=='y') speedVehicleControlStart();
        if(sd_buff[0]=='h') speedVehicleControlStop();

        if(sd_buff[4]=='a') speedSetEnginePIDReferenceValue ( (float)(atoi(sd_buff)) ) ;
        if(sd_buff[0]=='e') speedEngineControlStart();
        if(sd_buff[0]=='d') speedEngineControlStop();

        if(sd_buff[0]=='f') pedalsClutchRelease( 3000 );
        if(sd_buff[0]=='k') pedalsClutchCalibrate(0,5000,4000);
        if(sd_buff[0]=='l') pedalsClutchCalibrate(1,5000,4000);
        if(sd_buff[0]=='o') pedalsBrakeRelease( 3000 );
        if(sd_buff[0]=='i') pedalsBrakeCalibrate(0,5000,4000);
        if(sd_buff[0]=='p') pedalsBrakeCalibrate(1,5000,4000); //??? send negative pos!


       // chprintf( (BaseSequentialStream *)&SD3, " Speed %d\t EngSpeed  %d\t \n\r", 100, 53 );
        chprintf( (BaseSequentialStream *)&SD3, " Speed  %.2f %\t EngSpeed %.2f %\t Vref %.02f %\t  Eref %.02f %\t pidVal %d %\t BrakePos %d %\t ClutchPos %d %\t \n\r", (float)gazelGetSpeed(), (float)gazelGetEngineSpeed(), speedGetVehicleReference(),speedGetEngineReference(), speedGetPIDVal(), pedalsBrakeGetPosition(),pedalsClutchGetPosition() );






        for (int i = 0; i < 9; i++)
        {
          sd_buff[i]='?';
        }

        chThdSleepMilliseconds( 500 );
    }
}














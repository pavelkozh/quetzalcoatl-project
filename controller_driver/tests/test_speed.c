#include <tests.h>
#include <chprintf.h>

#include <speed.h>
#include <feedback.h>


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
    speedPIDInit();
    feedbackInit();


    uint8_t sd_buff[10];


    while(1) {

        //palToggleLine(LINE_LED1);

        sdReadTimeout( &SD3, sd_buff, 9, TIME_IMMEDIATE );


        if(sd_buff[4]=='s') speedSetVehiclePIDReferenceValue ( (float)(atoi(sd_buff)/10.0) ) ;
        if(sd_buff[0]=='y') speedSetVehicleControlStart();
        if(sd_buff[0]=='h') speedResetVehicleControlStart();

        if(sd_buff[4]=='a') speedSetEnginePIDReferenceValue ( (float)(atoi(sd_buff)) ) ;
        if(sd_buff[0]=='e') speedSetEngineControlStart();
        if(sd_buff[0]=='d') speedResetEngineControlStart();



       // chprintf( (BaseSequentialStream *)&SD3, " Speed %d\t EngSpeed  %d\t \n\r", 100, 53 );
        chprintf( (BaseSequentialStream *)&SD3, " Speed  %.02f %\t EngSpeed %.02f %\t Vref %.02f %\t  Eref %.02f %\t \n\r", gazelGetSpeed(), gazelGetEngineSpeed(), speedGetVehicleReference(),speedGetEngineReference() );






        for (int i = 0; i < 9; i++)
        {
          sd_buff[i]='?';
        }

        chThdSleepMilliseconds( 500 );
    }
}














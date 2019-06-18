#include <tests.h>
#include <chprintf.h>

#include <speed.h>
#include <MT.h>
#include <MT_control.h>



static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};



void TestMTControl ( void )
{
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX

    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );   //Led


    MTControlInit ();

    uint8_t sd_buff[10];
    int8_t current_gear = -1;

    while(1) {

        sdReadTimeout( &SD3, sd_buff, 9, TIME_IMMEDIATE );


        if(sd_buff[1]=='g') current_gear = mannualyShiftGear ( atoi(sd_buff) );
        if(sd_buff[0]=='y') setGearBoxControlEnableFlag();
        if(sd_buff[0]=='h') resetGearBoxControlEnableFlag();

        if(sd_buff[4]=='s') speedSetVehiclePIDReferenceValue ( atoi(sd_buff)/10.0 ) ;
        if(sd_buff[0]=='e') speedSetVehicleControlStart();

        if(sd_buff[0]=='d') speedResetVehicleControlStart();
        if(sd_buff[0]=='z') speedResetEngineControlStart();



        chprintf( (BaseSequentialStream *)&SD3, "current_gear %d\t flag %d\t  \n\r", current_gear, getGearBoxControlEnableFlag ());





        for (int i = 0; i < 9; i++)
        {
          sd_buff[i]='?';
        }

        chThdSleepMilliseconds( 500 );
    }
}














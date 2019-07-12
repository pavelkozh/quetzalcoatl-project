#include <tests.h>
#include <chprintf.h>

#include <engine_ignition.h>
#include <feedback.h>

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

void TestEngIgnition ( void )
{
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX

    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );   //Led


    engIgnitionInit();
    feedbackInit();

    uint8_t sd_buff[10];

    while(1) {

        sdReadTimeout( &SD3, sd_buff, 9, TIME_IMMEDIATE );


        if(sd_buff[0]=='a') engIgnitionSwitchOn ();
        if(sd_buff[0]=='s') engIgnitionSwitchOff();



        chprintf( (BaseSequentialStream *)&SD3, "Val %d\t \n\r", gazelGetEngineSpeed());





        for (int i = 0; i < 9; i++)
        {
          sd_buff[i]='?';
        }

        chThdSleepMilliseconds( 500 );
    }
}

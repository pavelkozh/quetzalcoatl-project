#include <tests.h>
#include <lld_sensor_module.h>
#include <chprintf.h>

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};


static uint16_t Ain_ch1 = 0, Ain_ch2 = 0;

void TestADCRouting ( void )
{
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOE, 8, PAL_MODE_ALTERNATE(8) );   // TX
    palSetPadMode( GPIOE, 7, PAL_MODE_ALTERNATE(8) );   // RX

    commonADC1UnitInit();



    while ( 1 )
    {
        Ain_ch1 = commonADC1UnitGetValue ( 1 );
        Ain_ch2 = commonADC1UnitGetValue ( 2 );

        chprintf( (BaseSequentialStream *)&SD3, "IN10:  %d\n\r", Ain_ch2 );
       /* chprintf( (BaseSequentialStream *)&SD7, "IN10:  %d\n","  ",
                  "IN10:  %d\n\r ", Ain_ch1, Ain_ch2 );

                  */

        chThdSleepMilliseconds( 10 );
    }

}


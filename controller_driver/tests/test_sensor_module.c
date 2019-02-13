#include <tests.h>
#include <lld_sensor_module.h>
#include <chprintf.h>

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};


static uint16_t Ain = 0;

void TestADCRouting ( void )
{
    sdStart( &SD7, &sdcfg );
    palSetPadMode( GPIOE, 8, PAL_MODE_ALTERNATE(8) );   // TX
    palSetPadMode( GPIOE, 7, PAL_MODE_ALTERNATE(8) );   // RX

    commonADC1UnitInit();



    while ( 1 )
    {
        Ain = commonADC1UnitGetValue ( 0 );
        chprintf( (BaseSequentialStream *)&SD7, "ADC1 IN10: %d\n\r", Ain );

        chThdSleepMilliseconds( 10 );
    }

}


#include <tests.h>
#include <lld_encoder.h>
#include <lld_control.h>
#include <chprintf.h>


static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};



void TestEncoderRouting (void)
{
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX

    chprintf( (BaseSequentialStream *)&SD3, " Start Test \r\n" );
    chThdSleepMilliseconds( 500 );

    lldEncoderInit();

    chprintf( (BaseSequentialStream *)&SD3, " Encoder Init finish \r\n" );
    chThdSleepMilliseconds( 500 );

    lldControlInit();

    chprintf( (BaseSequentialStream *)&SD3, " Control Init finish \r\n" );
    chThdSleepMilliseconds( 500 );


    rawEncoderValue_t encoder_ticks_cntr = 0;
    rawRevEncoderValue_t  revolutions_cntr = 0;
    bool rotation_dir = 0;

    while ( 1 )
    {
        encoder_ticks_cntr = lldGetEncoderRawTicks();
        rotation_dir       = lldGetEncoderDirection();
        revolutions_cntr   = lldGetEncoderRawRevs();

        motorRun();

        chprintf( (BaseSequentialStream *)&SD3, "rev: %d\t ticks: %d\t dir: %d\n\r",
                  (int16_t) (revolutions_cntr*100), encoder_ticks_cntr, rotation_dir );

        chThdSleepMilliseconds( 100 );

    }

}

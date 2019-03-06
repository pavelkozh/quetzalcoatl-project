#include <tests.h>
#include <lld_encoder.h>
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

    Position_t pos = 0.0;
    Velocity_t vel = 0.0;

    while ( 1 )
    {
        pos = encoderGetPosition ( );
        vel = encoderGetVelocity ( );
        chprintf( (BaseSequentialStream *)&SD3, "pos: %d\t vel: %d\t n\r", (int16_t) (pos*100), (int16_t) (vel*100) );

        chThdSleepMilliseconds( 10 );

    }
}

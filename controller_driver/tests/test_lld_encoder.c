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

    lldControlInit();
    lldEncoderInit();


    rawEncoderValue_t encoder_ticks_cntr = 0;
    rawRevEncoderValue_t  revolutions_cntr = 0;
    bool rotation_dir = 0;
    bool motor_run_enable = 0;

    while ( 1 )
    {
        encoder_ticks_cntr = lldGetEncoderRawTicks();
        rotation_dir       = lldGetEncoderDirection();
        revolutions_cntr   = lldGetEncoderRawRevs();

        motor_run_enable   =  ifDriverEnable ();



        if ( motor_run_enable )
        {
            pwmEnableChannel( &PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH ( &PWMD1, 5000 ) );
        }
        else
        {
            pwmEnableChannel( &PWMD1, 0, 0 );
        }

        chprintf( (BaseSequentialStream *)&SD3, "rev: %d\t ticks: %d\t dir: %d\n\r",
                  (int16_t) (revolutions_cntr*100), encoder_ticks_cntr, rotation_dir );

        chThdSleepMilliseconds( 10 );

    }
}

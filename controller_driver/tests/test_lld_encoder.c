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
    bool rotation_dir = 0, motor_run_enable = 0;

    while ( 1 )
    {
        chprintf( (BaseSequentialStream *)&SD3, "test \r\n" );
        chThdSleepMilliseconds( 100 );
        encoder_ticks_cntr = lldGetEncoderRawTicks();
        rotation_dir       = lldGetEncoderDirection();
        revolutions_cntr   = lldGetEncoderRawRevs();

        motor_run_enable   =  ifDriverEnable ();

        chprintf( (BaseSequentialStream *)&SD3, "en: %d \r\n", motor_run_enable );
        chThdSleepMilliseconds( 100 );

        if ( motor_run_enable )
        {
            uint32_t value = PWM_PERCENTAGE_TO_WIDTH ( &PWMD1, 5000 );

            chprintf( (BaseSequentialStream *)&SD3, "vl: %d \r\n", value );
            chThdSleepMilliseconds( 100 );

            pwmEnableChannel( &PWMD1, 0, value );
        }
        else
        {
            pwmEnableChannel( &PWMD1, 0, 0 );
        }

        chprintf( (BaseSequentialStream *)&SD3, "rev: %d\t ticks: %d\t dir: %d\n\r",
                  (int16_t) (revolutions_cntr*100), encoder_ticks_cntr, rotation_dir );

//        chprintf( (BaseSequentialStream *)&SD3, " test running! \r\n" );

        chThdSleepMilliseconds( 200 );

#if 0
        static int cntr=0;
        if (cntr++ > 50)
        {
            cntr = 0;
            palToggleLine(LINE_LED3);
        }
#endif

    }

}

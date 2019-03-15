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


    /* Variables */
    rawEncoderValue_t encoder_ticks_cntr = 0;
    rawRevEncoderValue_t  revolutions_cntr = 0;
    bool rotation_dir = 0;
    mVelocity_t m_speed = 0;
    int32_t rcv_value = 0;
    static uint8_t  sd_buff[10];

    while ( 1 )
    {
        encoder_ticks_cntr = lldGetEncoderRawTicks();
        rotation_dir       = lldGetEncoderDirection();
        revolutions_cntr   = lldGetEncoderRawRevs();
        m_speed            = encoderGetVelocity ();

        motorRun();



        chprintf( (BaseSequentialStream *)&SD3, "rev: %d\t ticks: %d\t dir: %d\t set speed: %d\t vel: %d\n\r",
                  (int16_t) (revolutions_cntr*100), encoder_ticks_cntr, rotation_dir, rcv_value, (int32_t)( m_speed * 100 ) );

        /* Read data from serial. Send value in range 000...100
         * First character used for sign,
         * but "atoi" cannot understand nothing but digits...  */
        sdReadTimeout( &SD3, sd_buff, 3, TIME_IMMEDIATE   );
        rcv_value = atoi(sd_buff);

        mSetSpeed ( rcv_value );

        chThdSleepMilliseconds( 100 );

    }

}

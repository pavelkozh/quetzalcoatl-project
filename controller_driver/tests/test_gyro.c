#include <tests.h>
#include <chprintf.h>
#include <lld_gyroscope.h>

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};


void testGyro ( void )
{
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX

    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );   //Led

    gyro_t angles = {
        .x_angle_deg = 0,
        .x_angle_deg = 0

    };

    gyroInit();

    while(1) {

        spiReceiveGyroXY();
        chprintf( (BaseSequentialStream *)&SD3, "x: %.2f; y: %.2f\n\r",getGyroX(),getGyroY());
        palToggleLine(LINE_LED2);

        chThdSleepMilliseconds( 20 );
    }
}

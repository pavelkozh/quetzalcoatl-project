#include <tests.h>
#include <emergency_stop.h>
#include <chprintf.h>
#include <pedals.h>

static const SerialConfig sdcfg = {
    .speed  = 9600,
    .cr1    = 0,
    .cr2    = 0,
    .cr3    = 0
};


/*
 * @brief test of emergency stop button
 * @note Implements all functions of emergency stop module, and also types button state and clutch pedal's position
 */
 void testEmergencyStop ( void ) {
 	sdStart( &SD3, &sdcfg );
     palSetPadMode( GPIOE, 8, PAL_MODE_ALTERNATE(8) );    // TX
     palSetPadMode( GPIOE, 7, PAL_MODE_ALTERNATE(8) );    // RX
     emergencyStopInit();
     while (1) {
        int pin = palReadLine( PAL_LINE( GPIOA, 0 ) );
        int32_t pos = pedalsClutchGetPosition();
        chprintf( (BaseSequentialStream *)&SD3, "Result %d\t Position %d  \n\r", pin, pos);
        chThdSleepSeconds( 1 );
     }
}

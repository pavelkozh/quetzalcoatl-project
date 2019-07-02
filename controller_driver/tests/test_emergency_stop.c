#include <tests.h>
#include <emergency_stop.h>
#include <chprintf.h> 

static const SerialConfig sdcfg = {
    .speed  = 9600,
    .cr1    = 0,
    .cr2    = 0,
    .cr3    = 0
};

 void testEmergencyStop ( void ) {
 	sdStart( &SD3, &sdcfg );
     palSetPadMode( GPIOE, 8, PAL_MODE_ALTERNATE(8) );    // TX
     palSetPadMode( GPIOE, 7, PAL_MODE_ALTERNATE(8) );    // RX
     emergencyStopInit();
     while (1) {
        int pin = palReadLine( PAL_LINE( GPIOA, 0 ) );
        chprintf( (BaseSequentialStream *)&SD3, "Result %d\t\n\r", pin);
        chThdSleepSeconds( 1 );
     }
}
#include <tests.h>
#include <steer.h>
#include <chprintf.h> 

static const SerialConfig sdcfg = {
    .speed  = 9600,
    .cr1    = 0,
    .cr2    = 0,
    .cr3    = 0
};

void testSteer ( void ) {
	sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOE, 8, PAL_MODE_ALTERNATE(8) );    // TX
    palSetPadMode( GPIOE, 7, PAL_MODE_ALTERNATE(8) );    // RX
    steerEncInit();
	while (1) {
		chprintf( (BaseSequentialStream *)&SD3, "Result %d\t \n\r",(uint16_t)(steerGetPosition()*100));
		chThdSleepSeconds( 1 );
	}
}


#include <tests.h>
#include <lld_steer.h>
#include <chprintf.h> 

static const SerialConfig sdcfg = {
    .speed  = 115200,
    .cr1    = 0, .cr2    = 0, .cr3    = 0
};

void testSteer ( void ) {
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX

    steerEncInit();
    steerMotorInit();

    uint8_t sd_buff[10];



	while (1) {
        palToggleLine(LINE_LED1);
	    sdReadTimeout( &SD3, sd_buff, 9, TIME_IMMEDIATE );

	    if(sd_buff[4]=='m') steerMotorSetSpeed( (float) (atoi(sd_buff)) );
	    if(sd_buff[0]=='s') steerMotorStartStopControl();
	    if(sd_buff[0]=='d') steerMotorDirChange();
        // palToggleLine(LINE_LED2);
        steerMotorStartStopControl();

		chprintf( (BaseSequentialStream *)&SD3, "Result %d\t \n\r",(uint16_t)(steerGetPosition()*100));
        // palToggleLine(LINE_LED3);

		for (int i = 0; i < 9; i++)
        {
          sd_buff[i]='?';
        }
		chThdSleepMilliseconds( 100 );
	}
}


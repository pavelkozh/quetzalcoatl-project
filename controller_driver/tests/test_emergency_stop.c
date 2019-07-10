#include <tests.h>
#include <emergency_stop.h>
#include <chprintf.h>
#include <pedals.h>

// static const SerialConfig sdcfg = {
//     .speed  = 9600,
//     .cr1    = 0,
//     .cr2    = 0,
//     .cr3    = 0
// };


/*
 * @brief test of emergency stop button
 * @note Implements all functions of emergency stop module, and also types button state and clutch pedal's position
 */
 void testEmergencyStop ( void ) {
 	//sdStart( &SD3, &sdcfg );
     palSetPadMode( GPIOE, 8, PAL_MODE_ALTERNATE(8) );    // TX
     palSetPadMode( GPIOE, 7, PAL_MODE_ALTERNATE(8) );    // RX
     emergencyStopInit();
     uint8_t sd_buff[10];
     while (1) {
        palToggleLine(LINE_LED3);
        int pin = palReadLine( PAL_LINE( GPIOA, 0 ) );
        int32_t pos = pedalsClutchGetPosition();
        chprintf( (BaseSequentialStream *)&SD3, "Result %d\t Position %d  \n\r", pin, pos);
        sdReadTimeout( &SD3, sd_buff, 9, TIME_IMMEDIATE );
        if(sd_buff[0]=='e') emergencyFullStop();
        for (int i = 0; i < 9; i++)
        {
            sd_buff[i]='?';
        }
        chThdSleepSeconds( 1 );
     }
}

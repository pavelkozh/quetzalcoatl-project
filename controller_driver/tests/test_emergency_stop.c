#include <tests.h>
#include <emergency_stop.h>
#include <chprintf.h>
#include <pedals.h>

 static const SerialConfig sdcfg = {
     .speed  = 115200,
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
     palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
     palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX

     emergencyStopInit();

     uint8_t sd_buff[10];

     while (1) {
        palToggleLine(LINE_LED3);
        sdReadTimeout( &SD3, sd_buff, 9, TIME_IMMEDIATE );

        uint8_t pin = palReadLine( PAL_LINE( GPIOA, 0 ) );



        if(sd_buff[0]=='e') emergencyFullStop();

        chprintf( (BaseSequentialStream *)&SD3, "Result %d\t ClutchPos %d\t BrakePos %d  \n\r", pin, pedalsClutchGetPosition(), pedalsBrakeGetPosition());

        for (int i = 0; i < 9; i++)
        {
            sd_buff[i]='?';
        }
        chThdSleepSeconds( 1 );
     }
}

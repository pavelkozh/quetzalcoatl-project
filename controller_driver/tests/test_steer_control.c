#include <tests.h>
#include <chprintf.h>

#include <steer_control.h>
#include <lld_steer.h>


static const SerialConfig sdcfg = {
    .speed  = 115200,
    .cr1    = 0, .cr2    = 0, .cr3    = 0
};

void testSteerPositionControl ( void ) {
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX

    steerInit();

    uint8_t sd_buff[10];

    while (1) {
        palToggleLine(LINE_LED1);

        sdReadTimeout( &SD3, sd_buff, 9, TIME_IMMEDIATE );

        if(sd_buff[4]=='m') steerSetPosition ( ((float) atoi(sd_buff))/10 );
        if(sd_buff[0]=='s') steerControlStart();
        if(sd_buff[0]=='d') steerControlStop();
        chprintf( (BaseSequentialStream *)&SD3, "PosRef %.02f\t Encoder %d\t PosErr %.02f\t PidOut %.02f\t MotorEn%d\t \n\r",steerDbgGetMotorPosRef(),(uint16_t)(steerGetPosition()), steerDbgGetMotorCalcPosErr(),steerDbgGetMotorCalcSpeedRef(), steerDbgGetEnableFlag());

        for (int i = 0; i < 9; i++)
        {
          sd_buff[i]='?';
        }
        chThdSleepMilliseconds( 1000 );
    }
}

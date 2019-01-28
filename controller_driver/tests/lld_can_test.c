#include <tests.h>
#include <lld_can.h>
#include <chprintf.h>



static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

void TestCanRouting ( void )
{
    sdStart( &SD7, &sdcfg );
    palSetPadMode( GPIOE, 8, PAL_MODE_ALTERNATE(8) );   // TX
    palSetPadMode( GPIOE, 7, PAL_MODE_ALTERNATE(7) );   // RX
    chprintf( (BaseSequentialStream *)&SD7, "Simulation enabled\n" );

    can_init();
    palSetPadMode( GPIOA, 0, PAL_MODE_OUTPUT_PUSHPULL );

    while (1)
    {
        for ( int i = 0; i < rxmsg.DLC; i++)
        {
            chprintf( (BaseSequentialStream *)&SD7,  ",%02X", rxmsg.data8[i] );
        }

        chThdSleepMilliseconds( 100 );

    }
}

#include <tests.h>
#include <hal_can_lld.h>
#include <chprintf.h>



static void simulation_init ( void )
{
    canStart(&CAND1,NULL);
    palSetPadMode( GPIOA, 0, PAL_MODE_OUTPUT_PUSHPULL );
    chThdCreateStatic(can_rx1_wa, sizeof(can_rx1_wa), NORMALPRIO + 7, can_rx, NULL);

    chprintf( (BaseSequentialStream *)&SD7, "Simulation enabled\n" );
}

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

void TestCanRouting ( void )
{
    sdStart( &SD7, &sdcfg );
    palSetPadMode( GPIOE, 8, PAL_MODE_ALTERNATE(8) );   // TX
    palSetPadMode( GPIOE, 7, PAL_MODE_ALTERNATE(7) );   // RX

    simulation_init();

    while (1)
    {
        for ( int i = 0; i < rxmsg.DLC; i++)
        {
            chprintf( (BaseSequentialStream *)&SD7,  ",%02X", rxmsg.data8[i] );
        }

        chThdSleepMilliseconds( 100 );

    }
}

#include <tests.h>
#include <lld_can.h>
#include <chprintf.h>
#include <term_graph.h>



static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

void TestCanRouting ( void )
{
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX
    chprintf( (BaseSequentialStream *)&SD3, "Simulation enabled\n" );

    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );
    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );
    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );
    can_init();
    set_display_atrib(F_RED);

    home();
    clrscr();
    chprintf( (BaseSequentialStream *)&SD3,  "Hello,         ! " ); 
    resetcolor();
    uint32_t i = 0;
    while (1)
    {
        gotoxy(7,1);
        set_display_atrib(F_RED);
        chprintf( (BaseSequentialStream *)&SD3,  "%d", i++); 
        resetcolor();
        chThdSleepMilliseconds( 100 );
    }
}

#include <tests.h>
#include <lld_can.h>
#include <chprintf.h>
#include <term_graph.h>



 extern  gazelParam gazel;


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

    home();
    clrscr();
    chprintf( (BaseSequentialStream *)&SD3,  "Engine Speed: \n\r"); 
    chprintf( (BaseSequentialStream *)&SD3,  "Driver's Demand Engine: \n\r");
    chprintf( (BaseSequentialStream *)&SD3,  "Actual Engine - Percent Torque: \n\r");
    chprintf( (BaseSequentialStream *)&SD3,  "Speed: \n\r"); 
    chprintf( (BaseSequentialStream *)&SD3,  "Accelerator Pedal Position: \n\r"); 
    chprintf( (BaseSequentialStream *)&SD3,  "Engine Oil Temperature: \n\r"); 
    chprintf( (BaseSequentialStream *)&SD3,  "Engine Fuel Temperature: \n\r"); 
    chprintf( (BaseSequentialStream *)&SD3,  "Position of doors: \n\r"); 
    chprintf( (BaseSequentialStream *)&SD3,  "Engine Fuel Rate: \n\r"); 
    chprintf( (BaseSequentialStream *)&SD3,  "Engine Instantaneous Fuel Economy: \n\r"); 
    chprintf( (BaseSequentialStream *)&SD3,  "Engine Throttle Valve : \n\r"); 
    chprintf( (BaseSequentialStream *)&SD3,  "Net Battery Current: \n\r"); 
    chprintf( (BaseSequentialStream *)&SD3,  "Alternator Current: \n\r"); 
    chprintf( (BaseSequentialStream *)&SD3,  "Alternator Current: \n\r"); 
    chprintf( (BaseSequentialStream *)&SD3,  "Electrical Potential (V): \n\r"); 
    chprintf( (BaseSequentialStream *)&SD3,  "Electrical Potential (V): \n\r"); 
    chprintf( (BaseSequentialStream *)&SD3,  "Break Pedal Position: \n\r"); 


    uint16_t i=0;
    while (1)
    {
    gotoxy(40,1);
    chprintf((BaseSequentialStream *)&SD3, "%.02f rpm/bit",gazel.EngineSpeed );
    gotoxy(40,2);
    chprintf((BaseSequentialStream *)&SD3, "%.02f rpm/bit",gazel.DriverIsDemandEnginePercentTorque );
    gotoxy(40,3);
    chprintf((BaseSequentialStream *)&SD3, "%.02f rpm/bit",gazel.ActualEnginePercentTorque );
    gotoxy(40,4);
    chprintf((BaseSequentialStream *)&SD3, "%.02f km/h",gazel.Speed );
    gotoxy(40,5);
    chprintf((BaseSequentialStream *)&SD3, "%.02f %",gazel.AcceleratorPedalPosition);
    gotoxy(40,6);
    chprintf((BaseSequentialStream *)&SD3, "%.04f deg C   ",gazel.EngineOilTemperature);
    gotoxy(40,7);
    chprintf((BaseSequentialStream *)&SD3, "%d deg C   ",gazel.EngineFuelTemperature);
    gotoxy(40,8);
    chprintf((BaseSequentialStream *)&SD3, "%d  ",gazel.Positionofdoors);
    gotoxy(40,9);
    chprintf((BaseSequentialStream *)&SD3, "%.02f L/h",gazel.EngineFuelRate);
    gotoxy(40,10);
    chprintf((BaseSequentialStream *)&SD3, "%.02f km/kg",gazel.EngineInstantaneousFuelEconomy);
    gotoxy(40,11);
    chprintf((BaseSequentialStream *)&SD3, "%.02f %%",gazel.EngineThrottleValve);
    gotoxy(40,12);
    chprintf((BaseSequentialStream *)&SD3, "%i A",gazel.NetBatteryCurrent);
    gotoxy(40,13);
    chprintf((BaseSequentialStream *)&SD3, "%d A",gazel.AlternatorCurrent);
    gotoxy(40,14);
    chprintf((BaseSequentialStream *)&SD3, "%.02f V",gazel.AlternatorPotential);
    gotoxy(40,15);
    chprintf((BaseSequentialStream *)&SD3, "%.02f V",gazel.ElectricalPotential);
    gotoxy(40,16);
    chprintf((BaseSequentialStream *)&SD3, "%.02f V   ",gazel.BatteryPotential);
    gotoxy(40,17);
    chprintf((BaseSequentialStream *)&SD3, "%.02f  %",gazel.BrakePedalPosition);
    chThdSleepMilliseconds( 100 );
    }
}

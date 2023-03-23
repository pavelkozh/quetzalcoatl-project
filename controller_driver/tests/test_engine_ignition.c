#include <tests.h>
#include <chprintf.h>

#include <engine_ignition.h>
#include <feedback.h>

#include <emergency_stop.h>
#include <sound_signal.h>

extern gazelParam *gaz;

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

void TestEngIgnition ( void )
{
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX

    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );   //Led


    engIgnitionInit();
    feedbackInit();
    soundSignalInit();

   // emergencyStopInit();

    uint8_t sd_buff[10];
    uint8_t sd_wr_buff[2]={5,3};
    uint8_t var = 15;

    while(1) {
        palToggleLine(LINE_LED1);
        float eng_speed = gazelGetEngineSpeed();
        float acc_pos = gazelGetAcceleratorPedalPosition();
        int8_t torque = gazelGetActualEnginePercentTorque();
        sdWriteTimeout(&SD3,(uint8_t*)&torque,sizeof(torque),100);
        //sdReadTimeout( &SD3, sd_buff, 9, TIME_IMMEDIATE );

        if(sd_buff[0]=='a') engIgnitionSwitchOn ();
        if(sd_buff[0]=='s') engIgnitionSwitchOff();
        if(sd_buff[0]=='b') engStarterSwitchOn();
        if(sd_buff[0]=='d') engStarterSwitchOff();
        if(sd_buff[0]=='q') soundSignalStartContiniousSignals();
        if(sd_buff[0]=='w') soundSignalStopContiniousSignals();

        if(sd_buff[4]=='e') engIgnitionDbgSetEngSpeed (atoi(sd_buff));

        for (int i = 0; i < 9; i++)
        {
          sd_buff[i]='?';
        }

//


        //chprintf( (BaseSequentialStream *)&SD3, "EngSpeed %.02f\t ThreadFlag %d\t TestFlag %d\t TestVar %d\t \n\r", gazelGetEngineSpeed(), engIgnitionDbgGetThreadEnFlag(), engIgnitionDbgGetFlaf (), var);

        for (int i = 0; i < 9; i++)
        {
          sd_buff[i]='?';
        }

        chThdSleepMilliseconds( 200 );
    }
}

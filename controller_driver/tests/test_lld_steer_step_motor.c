#include <tests.h>
#include <chprintf.h>

#include <lld_steer_step_motor.h>

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

#define SPEED_REF_CHANGE_STEP      500

void TestSteerSM ( void )
{
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX

    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    //palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );   //Led


    lldSteerSMInit();

    uint8_t sd_buff[10];
    uint16_t speed_ref =  3000;

    while(1) {

        palToggleLine(LINE_LED2);
       // palToggleLine(LINE_LED3);

        sdReadTimeout( &SD3, sd_buff, 9, TIME_IMMEDIATE );


        if(sd_buff[0]=='c') { lldSteerSMStop(); }
        if(sd_buff[5]=='a') lldSteerSMSetPosition (atoi(sd_buff), 10000);
        if(sd_buff[6]=='z') lldSteerSMMoveToTheRight ( atoi(sd_buff) );
        if(sd_buff[6]=='x') lldSteerSMMoveToTheLeft  ( atoi(sd_buff) );
        if(sd_buff[0]=='f') speed_ref += SPEED_REF_CHANGE_STEP;
        if(sd_buff[6]=='e') speed_ref -= SPEED_REF_CHANGE_STEP;

        if(sd_buff[0]=='p') lldSteerSMSetPosition (-2000, 10000);
        if(sd_buff[0]=='k') lldSteerSMSetPosition (2000, 10000);






        chprintf( (BaseSequentialStream *)&SD3, "pos %d\t state %d\t mode %d\t speed %d\t   \n\r", lldSteerSMGetPosition (),lldSteerSMGetState (), lldSteerSMGetMode (), lldSteerSMGetSpeed ());





        for (int i = 0; i < 9; i++)
        {
          sd_buff[i]='?';
        }

        chThdSleepMilliseconds( 500 );
    }
}


#include <tests.h>
#include <chprintf.h>

#include <pedals.h>
#include <feedback.h>

extern gazelParam *gaz;

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};


void TestPedals ( void )
{
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX

    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );   //Led

    pedalsInit();
    feedbackInit();

    pedalsAcceleratorControl(57);

    uint8_t sd_buff[10];
    sdGet(&SD3);// ожидание символа от GUI для синхронизации
    while(1) {

        float eng_speed = gazelGetEngineSpeed();
        int8_t torque = gazelGetActualEnginePercentTorque();
        float acc_pos = gazelGetAcceleratorPedalPosition();
//        float speed = gazelGetSpeed();

        double speed_px4flow_filtered = gaz->Speed_px4flow;
//        double speed_px4flow = gaz->Speed_px4flow_nonfilter;
        double speed_can = gaz->Speed;


        //Отправка данных
//        sdWrite(&SD3,(uint8_t*)&eng_speed,sizeof(eng_speed));
//        sdWrite(&SD3,(uint8_t*)&torque,sizeof(torque));
//        sdWrite(&SD3,(uint8_t*)&acc_pos,sizeof(acc_pos));
////        sdWrite(&SD3,(uint8_t*)&speed,sizeof(speed));
//        sdWrite(&SD3,(uint8_t*)&speed_px4flow_filtered,sizeof(speed_px4flow_filtered));
////        sdWrite(&SD3,(uint8_t*)&speed_px4flow,sizeof(speed_px4flow));
//        sdWrite(&SD3,(uint8_t*)&speed_can,sizeof(speed_can));

        sdReadTimeout( &SD3, sd_buff, 9, TIME_IMMEDIATE );//прием команды
        palToggleLine(LINE_LED2);
        //chprintf( (BaseSequentialStream *)&SD3, "v: %f\t\n\r",speed_px4flow);
        if(sd_buff[0]=='c') { pedalsClutchStop(); pedalsBrakeStop(); }

        /* Clutch test*/
        if(sd_buff[0]=='x') pedalsClutchPress( 500 );
        if(sd_buff[0]=='f') pedalsClutchRelease( 3000 );
        if(sd_buff[0]=='a') pedalsClutchInitialization();
        if(sd_buff[6]=='e') pedalsClutchMove(atoi(sd_buff),3000);
        if(sd_buff[5]=='y') pedalsClutchCalibrate(0, 3000, atoi(sd_buff));
        if(sd_buff[5]=='q') pedalsClutchCalibrate(1, 3000, atoi(sd_buff));
        if(sd_buff[6]=='n') pedalsClutchChangeSpeed ( atoi(sd_buff) );

        /* Brake test*/
        if(sd_buff[0]=='z') pedalsBrakePress( 3000 );
        if(sd_buff[0]=='o') pedalsBrakeRelease( 3000 );
        if(sd_buff[0]=='m') pedalsBrakeInitialization();
        if(sd_buff[6]=='r') pedalsBrakeMove(atoi(sd_buff), 3000);
        if(sd_buff[5]=='h') pedalsBrakeCalibrate (0, 3000, atoi(sd_buff));
        if(sd_buff[5]=='w') pedalsBrakeCalibrate (1, 3000, atoi(sd_buff));
        if(sd_buff[6]=='g') pedalsBrakeChangeSpeed ( atoi(sd_buff) );

        chprintf( (BaseSequentialStream *)&SD3, "ClState %d\t ClMode %d\t ClStatePos %d\t ClSpeed %d\t BrState %d\t BrMode %d\t BrStatePos %d\t BrSpeed %d\t \n\r",gazelGetClutchSwitch(), pedalsClutchGetMode(), pedalsClutchGetPosition(), pedalsClutchGetSpeed(),gazelGetBrakeSwitch(), pedalsBrakeGetMode(), pedalsBrakeGetPosition(), pedalsBrakeGetSpeed());
        //chprintf( (BaseSequentialStream *)&SD3, "BrState %d\t BrMode %d\t BrStatePos %d\t BrSpeed %d\t \n\r",pedalsBrakeGetState(), pedalsBrakeGetMode(), pedalsBrakeGetPosition(), pedalsBrakeGetSpeed());

        /* Accelerator pedal test */
        if(sd_buff[4]=='u') pedalsAcceleratorControl ( atoi(sd_buff) );
        //chprintf( (BaseSequentialStream *)&SD3, "rot: %f torque:%d acc_pos: %f\t\n\r",eng_speed,torque,acc_pos);

        for (int i = 0; i < 9; i++)
        {
          sd_buff[i]='?';
        }

        chThdSleepMilliseconds(50);
    }
}














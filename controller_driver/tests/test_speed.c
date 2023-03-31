#include <tests.h>
#include <chprintf.h>

#include <speed.h>
#include <feedback.h>
//#include <pedals.h>

extern gazelParam *gaz;

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

void TestSpeed ( void )
{
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX

    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );   //Led

    //palSetLine(LINE_LED2);
    speedInit();
    feedbackInit();
    pedalsInit();

    pedalsAcceleratorControl(0);

    uint8_t sd_buff[10];

    sdGet(&SD3);// ожидание символа от GUI для синхронизации

    while(1) {

        float eng_speed = gazelGetEngineSpeed();
        float acc_pos = gazelGetAcceleratorPedalPosition();
        float torque = gazelGetActualEnginePercentTorque();
        float E_reference = speedGetEngineReference();
        float speed_px4flow_filtered = gaz->Speed_px4flow;
        float speed_can = gaz->Speed;

        //Отправка данных
        sdWrite(&SD3,(uint8_t*)&eng_speed,sizeof(eng_speed));
        sdWrite(&SD3,(uint8_t*)&torque,sizeof(torque));
        sdWrite(&SD3,(uint8_t*)&acc_pos,sizeof(acc_pos));
        sdWrite(&SD3,(uint8_t*)&E_reference,sizeof(E_reference));
        sdWrite(&SD3,(uint8_t*)&speed_px4flow_filtered,sizeof(speed_px4flow_filtered));
        sdWrite(&SD3,(uint8_t*)&speed_can,sizeof(speed_can));

        sdReadTimeout( &SD3, sd_buff, 9, TIME_IMMEDIATE );
        palToggleLine(LINE_LED2);

        if(sd_buff[4]=='s') speedSetVehiclePIDReferenceValue ( (float)(atoi(sd_buff)/10.0) ) ;
        if(sd_buff[0]=='y') speedVehicleControlStart();
        if(sd_buff[0]=='h') speedVehicleControlStop();

        if(sd_buff[4]=='a') speedSetEnginePIDReferenceValue ( (float)(atoi(sd_buff)) ) ;
        if(sd_buff[0]=='e') speedEngineControlStart();
        if(sd_buff[0]=='d') speedEngineControlStop();

        if(sd_buff[0]=='f') pedalsClutchRelease( 3000 );
        if(sd_buff[0]=='k') pedalsClutchCalibrate(0,5000,4000);
        if(sd_buff[0]=='l') pedalsClutchCalibrate(1,5000,4000);
        if(sd_buff[0]=='o') pedalsBrakeRelease( 3000 );
        if(sd_buff[0]=='i') pedalsBrakeCalibrate(0,5000,4000);
        if(sd_buff[0]=='p') pedalsBrakeCalibrate(1,5000,4000); //??? send negative pos!

        if(sd_buff[4]=='u') pedalsAcceleratorControl ( atoi(sd_buff) );

        //chprintf( (BaseSequentialStream *)&SD3, " M %.02f EngSpeed  %.02f\t \n\r", gazelGetActualEnginePercentTorque(), gazelGetEngineSpeed() );
        //chprintf( (BaseSequentialStream *)&SD3, " Speed  %.02f %\t EngSpeed %.02f %\t Vref %.02f %\t  Eref %.02f %\t pidVal %.02f %\t BrakePos %d %\t  \n\r", speedDbgGazelSpeed(), speedDbgGazelEngSpeed(), speedGetVehicleReference(),speedGetEngineReference(), speedGetPIDVal(), speedDbgBrakePos() );

        for (int i = 0; i < 9; i++)
        {
          sd_buff[i]='?';
        }

        chThdSleepMilliseconds( 50 );
    }
}














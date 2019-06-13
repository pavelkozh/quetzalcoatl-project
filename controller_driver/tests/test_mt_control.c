#include <tests.h>
#include <chprintf.h>


static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};



void TestMtControl ( void )
{
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX

    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );   //Led


    uint8_t gear = -1;

    while(1) {

        sdReadTimeout( &SD3, sd_buff, 9, TIME_IMMEDIATE );

        if(sd_buff[6]=='q')
        {
            gear = -1;
            setTrackedModePositionVerticalMotor ( atoi(sd_buff) );
        }

        if(sd_buff[6]=='w')
        {
            gear = -1;
            setTrackedModePositionGorisontalMotor ( atoi(sd_buff) );
        }

        if(sd_buff[0]=='c') { stopClutchPedal (); stopBrakePedal (); }
        if(sd_buff[0]=='z') pressBrakePedal ( 3000 );
        if(sd_buff[0]=='o') releaseClutchPedal( 3000 );
        if(sd_buff[0]=='x') pressBrakePedal( 3000 );
        if(sd_buff[0]=='f') releaseBrakePedal( 3000 );


        //**** Gear Shift commands ****//
        if(sd_buff[0]=='e') gear = 0;
        if(sd_buff[0]=='d') gear = 1;
        if(sd_buff[0]=='r') gear = 2;
        if(sd_buff[0]=='g') gear = 6;
        //if(sd_buff[0]=='y') gear_shift_control = 1;
        //if(sd_buff[0]=='h') gear_shift_control = 0;



        //chprintf( (BaseSequentialStream *)&SD3, "EControlEn %d\tGControlEn %d\tVControlEn %d\tVref: %.2f\tVspeed: %.2f\tESpeed: %.02f\tGN %d\MVpos %d\MGpos %d\tCS %d\tBS %d\t ClPos %d\t \r\n", engine_control_start, gear_shift_control, vehicle_control_start, Vref, gazel.Speed, gazel.EngineSpeed, gear_num, v_motor_position, g_motor_position, gazel.ClutchSwitch, gazel.BrakeSwitch, ClutchM.position );



        //*****Calibration testing*****//
        #if 0
        if(sd_buff[0]=='e') all_sensors_state = doCalibrationMT ();
        all_sensors_state = doCalibrationMT ();

        chprintf( (BaseSequentialStream *)&SD3, "vupLS_state:  %d vlowLS_state:  %d hlLS_state:  %d hrLS_state :  %d  v_pos:  %d  h_pos:  %d  vm_state: %d hm_state: %d \r\n",(all_sensors_state)&&(1<<3),(all_sensors_state)&&(1<<2),(all_sensors_state)&&(1<<1) ,(all_sensors_state)&&(1<<0) , v_motor_position, g_motor_position, vm_state, gm_state);
        #endif


        for (int i = 0; i < 9; i++)
        {
          sd_buff[i]='?';
        }
        chThdSleepMilliseconds( 500 );
    }
}














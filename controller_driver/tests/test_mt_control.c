#include <tests.h>
#include <lld_control.h>
#include <chprintf.h>
#include <mt_control.h>

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};






void TestMtControl ( void )
{
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX


    mtControlInit ();

    static char  sd_buff[10] ;
    uint16_t speed = 2500;

    int32_t     v_pos = 0,
                g_pos = 0,
                v_max_pos = 0,
                g_max_pos = 0,
                v_target_pos = 0,
                g_target_pos = 0;

    bool        v_state = false,
                g_state = false;

    uint8_t     v_mode = 0,
                g_mode = 0;

    uint16_t    v_speed = 0,
                g_speed = 0;

    char        command = ' ';



    while(1)
    {

        sdReadTimeout( &SD3, sd_buff, 6, TIME_IMMEDIATE   );

        v_pos        = getVerticalPosition ();
        g_pos        = getGorisontalPosition ();
        v_max_pos    = getVerticalMaxPosition ();
        g_max_pos    = getGorisontalMaxPosition ();
        v_target_pos = getVerticalTargetPosition ();
        g_target_pos = getGorisontalTargetPosition ();
        v_state      = getVerticalState ();
        g_state      = getGorisontalState ();
        v_mode       = getVerticalMode ();
        g_mode       = getGorisontalMode ();
        v_speed      = getVerticalSpeed ();
        g_speed      = getGorisontalSpeed ();


//        //*****MOTOR CONTROL*******//
//
//        if(sd_buff[5]=='v') {
//          speed = atoi(sd_buff);
//          MotorSetSpeed( &m_vertical, speed);
//        }
//
//
//
//        if(sd_buff[0]=='r') MotorRunContinuous( &m_vertical, 1, speed);
//        if(sd_buff[0]=='f') MotorRunContinuous( &m_vertical, 0, speed);
//        if(sd_buff[0]=='t') MotorRunContinuous( &m_gorisontal, 1, speed);
//        if(sd_buff[0]=='g') MotorRunContinuous( &m_gorisontal, 0, speed);
//
//        if(sd_buff[0]=='u') { MotorRunTracking( &m_vertical, speed); MotorRunTracking( &m_gorisontal, speed);}
//
//        if(sd_buff[5]=='q') m_vertical.tracked_position = atoi(sd_buff);
//        if(sd_buff[5]=='w') m_gorisontal.tracked_position = atoi(sd_buff);
//
//        if(sd_buff[0]=='c') { MotorStop( &m_vertical );  MotorStop( &m_gorisontal ); }
//
//
        if(sd_buff[5]=='e') { command = ' '; verticalCaclibration (1, speed, atoi(sd_buff));}
        if(sd_buff[5]=='d') { command = ' '; verticalCaclibration (0, speed, atoi(sd_buff));}
        if(sd_buff[5]=='y') { command = ' '; gorisontalCaclibration (1, speed, atoi(sd_buff));}
        if(sd_buff[5]=='h') { command = ' '; gorisontalCaclibration (0, speed, atoi(sd_buff));}
//
//        if(sd_buff[5]=='z') m_vertical.max_position = atoi(sd_buff);
//        if(sd_buff[5]=='x') m_gorisontal.max_position = atoi(sd_buff);



        /* Gear selection */
        if(sd_buff[0]=='n')
        {
            command = 'n';
            //shiftMTToNeutral ();
        }

        if(sd_buff[0]=='1')
        {
            command = '1';
            //shiftToFirstGear ();
        }

        if(sd_buff[0]=='2')
        {
            command = '2';
            //shiftToSecondGear ();
        }

        if(sd_buff[0]=='3')
        {
            command = '3';
            //shiftToThirdGear ();
        }

        if(sd_buff[0]=='4')
        {
            command = '4';
            //shiftToForthGear ();
        }

        if(sd_buff[0]=='5')
        {
            command = '5';
            //shiftToFifthGear ();
        }

        if(sd_buff[0]=='p') // back
        {
            command = 'p';
            //shiftToReverseGear ();
        }

        switch ( command )
        {
            case 'n':  shiftMTToNeutral (); break;
            case '1':  shiftMTToNextGear (1, 4000); break;
            case '2':  shiftMTToNextGear (2, 4000); break;
            case '3':  shiftMTToNextGear (3, 4000); break;
            case '4':  shiftMTToNextGear (4, 4000); break;
            case '5':  shiftMTToNextGear (5, 4000); break;
            case 'p':  shiftMTToNextGear (6, 4000); break;

        }



        chprintf( (BaseSequentialStream *)&SD3, "State_v: %d State_g: %d  Mode_v: %d Mode_g: %d  "
                "Position_v: %d  Position_g: %d Max_v: %d Max_g: %d Track_v: %d Track_g: %d command %c\r\n",
                v_state,g_state , v_mode,g_mode, v_pos ,g_pos ,v_max_pos ,
                g_max_pos , v_target_pos, g_target_pos, command);

        for (int i = 0; i < 9; i++)
       {
         sd_buff[i]='?';
       }
        chThdSleepMilliseconds( 500 );


    };
}

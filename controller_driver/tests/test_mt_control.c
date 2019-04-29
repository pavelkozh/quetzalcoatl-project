#include <tests.h>
#include <lld_control.h>
#include <chprintf.h>

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

void risingEdgeMTVerticalCallback(PWMDriver *pwmd);
void fallingEdgeMTVerticalCallback(PWMDriver *pwmd);

void risingEdgeMTGorisontalCallback(PWMDriver *pwmd);
void fallingEdgeMTGorisontalCallback(PWMDriver *pwmd);


MotorDriver m_vertical = {
                          .pwmd              = &PWMD11,
                          .dir_line          = PAL_LINE (GPIOG, 1 ),
                          .rising_edge_cb    = risingEdgeMTVerticalCallback,
                          .falling_edge_cb   = fallingEdgeMTVerticalCallback,
                          .max_position      = 4000
};

void risingEdgeMTVerticalCallback(PWMDriver *pwmd)
{

    (void) pwmd;
    risingEdgeCb(&m_vertical);

}
void fallingEdgeMTVerticalCallback(PWMDriver *pwmd)
{

    (void) pwmd;
    fallingEdgeCb(&m_vertical);
}


MotorDriver m_gorisontal = {
                          .pwmd              = &PWMD14,
                          .dir_line          = PAL_LINE (GPIOE, 3 ),
                          .rising_edge_cb    = risingEdgeMTGorisontalCallback,
                          .falling_edge_cb   = fallingEdgeMTGorisontalCallback,
                          .max_position      = 4000
};

void risingEdgeMTGorisontalCallback(PWMDriver *pwmd)
{

    (void) pwmd;
    risingEdgeCb(&m_gorisontal);

}
void fallingEdgeMTGorisontalCallback(PWMDriver *pwmd)
{

    (void) pwmd;
    fallingEdgeCb(&m_gorisontal);
}

static char  sd_buff[10] ;
uint16_t speed = 4000;

void TestMtControl ( void )
{
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX


    /*MotoR driver Setting */
    palSetLineMode( PAL_LINE( GPIOF, 7),  PAL_MODE_ALTERNATE(3) );
    palSetLineMode( m_vertical.dir_line, PAL_MODE_OUTPUT_PUSHPULL);

    MotorlldControlInit( &m_vertical );

    palSetLineMode( PAL_LINE( GPIOF, 9),  PAL_MODE_ALTERNATE(9) );
    palSetLineMode( m_gorisontal.dir_line, PAL_MODE_OUTPUT_PUSHPULL);
    MotorlldControlInit( &m_gorisontal );




    while(1){
        sdReadTimeout( &SD3, sd_buff, 6, TIME_IMMEDIATE   );


        //*****MOTOR CONTROL*******//

        if(sd_buff[5]=='n') {
          speed = atoi(sd_buff);
          MotorSetSpeed( &m_vertical, speed);
        }



        if(sd_buff[0]=='r') MotorRunContinuous( &m_vertical, 1, speed);
        if(sd_buff[0]=='f') MotorRunContinuous( &m_vertical, 0, speed);
        if(sd_buff[0]=='t') MotorRunContinuous( &m_gorisontal, 1, speed);
        if(sd_buff[0]=='g') MotorRunContinuous( &m_gorisontal, 0, speed);

        if(sd_buff[0]=='u') { MotorRunTracking( &m_vertical, speed); MotorRunTracking( &m_gorisontal, speed);}

        if(sd_buff[5]=='q') m_vertical.tracked_position = atoi(sd_buff);
        if(sd_buff[5]=='w') m_gorisontal.tracked_position = atoi(sd_buff);

        if(sd_buff[0]=='c') { MotorStop( &m_vertical );  MotorStop( &m_gorisontal ); }


        if(sd_buff[5]=='e') MotorRunCaclibration( &m_vertical, 1, speed, atoi(sd_buff) );
        if(sd_buff[5]=='d') MotorRunCaclibration( &m_vertical, 0, speed, atoi(sd_buff) );
        if(sd_buff[5]=='y') MotorRunCaclibration( &m_gorisontal, 1, speed, atoi(sd_buff) );
        if(sd_buff[5]=='h') MotorRunCaclibration( &m_gorisontal, 0, speed, atoi(sd_buff) );

        if(sd_buff[5]=='z') m_vertical.max_position = atoi(sd_buff);
        if(sd_buff[5]=='x') m_gorisontal.max_position = atoi(sd_buff);

        chprintf( (BaseSequentialStream *)&SD3, "State_v: %d State_g: %d  Mode_v: %d Mode_g: %d  Position_v: %d  Position_g: %d Max_v: %d Max_g: %d Track_v: %d Track_g: %d \r\n",m_vertical.state,m_gorisontal.state , m_vertical.mode,m_gorisontal.mode, m_vertical.position ,m_gorisontal.position ,m_vertical.max_position ,m_gorisontal.max_position , m_vertical.tracked_position,m_gorisontal.tracked_position);

        for (int i = 0; i < 9; i++)
       {
         sd_buff[i]='?';
       }
        chThdSleepMilliseconds( 500 );


    };
}

#include <tests.h>
#include <lld_control.h>
#include <lld_sensor_module.h>
#include <chprintf.h>

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

static bool right_dir = 0, left_dir = 0, drive_enable = 0;
static uint16_t Ain = 0;
static float duty_cucle = 0;


static void extcb( EXTDriver *extp, expchannel_t channel )
{
    extp = extp;
    //channel = channel;
    if ( channel == 4 )
    {
        if ( palReadPad ( GPIOE, 4 ) )
        {
            drive_enable = 1;
            palClearLine( LINE_LED1 );
        }
        else
        {
            drive_enable = 0;
            palSetLine( LINE_LED1 );
        }
    }
    if ( channel == 5 )
    {
        if ( palReadPad (GPIOE, 5) )
        {
            left_dir = 0;
            right_dir = 1;
            palClearLine( LINE_LED3 );
            palSetLine( LINE_LED2 );

        }
        else
        {
            right_dir = 0;
            left_dir = 1;
            palClearLine( LINE_LED2 );
            palSetLine( LINE_LED3 );

        }

    }
    /*
    if ( channel == 6 )
    {
        right_dir = 0;
        left_dir = 1;
        palToggleLine( LINE_LED3 );
    }
    */

    //palToggleLine( LINE_LED1 );
}

static const EXTConfig extcfg = {
  .channels =
  {
    [0]  = {EXT_CH_MODE_DISABLED, NULL},
    [1]  = {EXT_CH_MODE_DISABLED, NULL},
    [2]  = {EXT_CH_MODE_DISABLED, NULL},
    [3]  = {EXT_CH_MODE_DISABLED, NULL},
    [4]  = {EXT_CH_MODE_BOTH_EDGES   | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOE, extcb}, //PE4
    [5]  = {EXT_CH_MODE_BOTH_EDGES   | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOE, extcb}, //PE5
    [6]  = {EXT_CH_MODE_DISABLED, NULL},
    [7]  = {EXT_CH_MODE_DISABLED, NULL},
    [8]  = {EXT_CH_MODE_DISABLED, NULL},
    [9]  = {EXT_CH_MODE_DISABLED, NULL},
    [10] = {EXT_CH_MODE_DISABLED, NULL},
    [11] = {EXT_CH_MODE_DISABLED, NULL},
    [12] = {EXT_CH_MODE_DISABLED, NULL},
    [13] = {EXT_CH_MODE_DISABLED, NULL},
    [14] = {EXT_CH_MODE_DISABLED, NULL},
    [15] = {EXT_CH_MODE_DISABLED, NULL},
  }
};


void TestPWMRouting (void)
{
  PWMUnitInit();
  commonADC1UnitInit();
  extStart( &EXTD1, &extcfg );

  sdStart( &SD7, &sdcfg );
  palSetPadMode( GPIOE, 8, PAL_MODE_ALTERNATE(8) );   // TX
  palSetPadMode( GPIOE, 7, PAL_MODE_ALTERNATE(8) );   // RX

  palSetPadMode( GPIOE, 4, PAL_MODE_OUTPUT_OPENDRAIN );
  palSetPadMode( GPIOE, 5, PAL_MODE_OUTPUT_OPENDRAIN );




  while (1)
  {
      Ain = commonADC1UnitGetValue ( 1 );
      duty_cucle = (float)Ain * 10000.0 / 4096.0;



      if ( drive_enable )
      {
          pwmEnableChannel( &PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH (&PWMD1, (uint16_t)(duty_cucle) ) );
          //pwmEnableChannel( &PWMD1, 0, 100 );
      }
#if 0
      else
      {
          pwmEnableChannel( &PWMD1, 0, 0 );
      }

#endif

      chprintf( (BaseSequentialStream *)&SD7, "ADC:  %d\t En %d\t DirR %d\t DirL %d\t Duty %d\n\r", Ain, drive_enable, right_dir, left_dir, (uint16_t) (duty_cucle *10) );

      //palReadPad(GPIOE, 6);


      chThdSleepMilliseconds( 100 );
  }
}

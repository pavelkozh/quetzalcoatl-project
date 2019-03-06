#include <tests.h>
#include <lld_control.h>
#include <lld_sensor_module.h>
#include <chprintf.h>




static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

//static bool right_dir = 0, left_dir = 0, drive_enable = 0, drive_start = 0;
static uint16_t Ain = 0, period_s = 0;
static float duty_cucle = 0.0;
static uint8_t rcv_data = 0, sd_buff[10];
int8_t *sd_value;

#if 0
static void extcb( EXTDriver *extp, expchannel_t channel )
{
    /* to avoid warnings */
    extp = extp;

    /* Driver ENABLE button (latching) change state:
     * (0->1: drive enable)
     * (1->0: drive disable)
     */
    if ( channel == DRIVE_ENABLE_BUTTON_PIN )
        {
            if ( palReadPad ( DRIVE_ENABLE_BUTTON_PORT, DRIVE_ENABLE_BUTTON_PIN ) )
            {
                drive_enable = 0;

                /* to indicate "enable" button state */
                palClearLine( LINE_LED1 );

                /* to control driver's "enable" pin */
                palSetPad ( CONTROL_DRIVE_ENABLE_PORT, CONTROL_DRIVE_ENABLE_PIN );
            }
            else
            {
                drive_enable = 1;

                /* to indicate "enable" button state */
                palSetLine( LINE_LED1 );

                /* to control driver's "enable" pin */
                palClearPad ( CONTROL_DRIVE_ENABLE_PORT, CONTROL_DRIVE_ENABLE_PIN );
            }
        }

    /* Driver START/STOP button (latching) change state:
     * (0->1: drive start)
     * (1->0: drive stop )
     */
    if ( channel == DRIVE_START_BUTTON_PIN )
    {
        if ( palReadPad ( DRIVE_START_BUTTON_PORT, DRIVE_START_BUTTON_PIN ) )
        {
            drive_start = 1;

            /* to indicate "enable" button state */
            //palClearLine( LINE_LED1 );

            /* to control driver's "enable" pin */
            //palSetPad ( CONTROL_DRIVE_ENABLE_PORT, CONTROL_DRIVE_ENABLE_PIN );
        }
        else
        {
            drive_start = 0;

            /* to indicate "enable" button state */
            //palSetLine( LINE_LED1 );

            /* to control driver's "enable" pin */
            //palClearPad ( CONTROL_DRIVE_ENABLE_PORT, CONTROL_DRIVE_ENABLE_PIN );
        }
    }

    /* Driver DIRECTION toggle button changing state occure:
     * (0->1: right direction)
     * (1->0: left direction )
     */
    if ( channel == DRIVE_DIR_BUTTON_PIN )
    {
        if ( palReadPad (DRIVE_DIR_BUTTON_PORT, DRIVE_DIR_BUTTON_PIN) )
        {
            left_dir = 0;
            right_dir = 1;

            /* to indicate "direction" button state */
            palClearLine( LINE_LED3 );
            palSetLine( LINE_LED2 );

            /* to control driver's "direction" pin */
            palSetPad ( CONTROL_DRIVE_DIR_PORT, CONTROL_DRIVE_DIR_PIN );

        }
        else
        {
            right_dir = 0;
            left_dir = 1;

            /* to indicate "direction" button state */
            palClearLine( LINE_LED2 );
            palSetLine( LINE_LED3 );

            /* to control driver's "direction" pin */
            palClearPad (  CONTROL_DRIVE_DIR_PORT, CONTROL_DRIVE_DIR_PIN );

        }

    }
}
#endif

#if 0
/* EXT Driver configuration */
static const EXTConfig extcfg = {
  .channels =
  {
    [0]  = {EXT_CH_MODE_DISABLED, NULL},
    [1]  = {EXT_CH_MODE_DISABLED, NULL},
    [2]  = {EXT_CH_MODE_DISABLED, NULL},
    [3]  = {EXT_CH_MODE_DISABLED, NULL},
    [4]  = {EXT_CH_MODE_BOTH_EDGES   | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOE, extcb}, //PE4
    [5]  = {EXT_CH_MODE_BOTH_EDGES   | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOE, extcb}, //PE5
    [6]  = {EXT_CH_MODE_BOTH_EDGES   | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOE, extcb}, //PE6
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
#endif

#if 0
void testPWMRoutingInit ( void )
{

}
#endif

void TestPWMRouting (void)
{
  /* Serial driver configuration*/
  sdStart( &SD3, &sdcfg );
  palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
  palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX


 // PWMUnitInit();
  commonADC1UnitInit();
  lldCcontrolInit();

  bool motor_run_enable = 0;

#if 0
  /* Define channel config structure */
  EXTChannelConfig ch_conf;

  /* Fill in configuration for channel */
  ch_conf.mode  = EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOE;
  ch_conf.cb    = extcb;

  /*EXT driver initialization*/
  commonExtDriverInit();

  /* Set channel (second arg) mode with filled configuration */
  extSetChannelMode( &EXTD1, DRIVE_START_BUTTON_PIN, &ch_conf );
  extSetChannelMode( &EXTD1, DRIVE_DIR_BUTTON_PIN, &ch_conf );
  extSetChannelMode( &EXTD1, DRIVE_ENABLE_BUTTON_PIN, &ch_conf );

  /* Set up EXT channels hardware pin mode as digital input  */
  palSetPadMode( GPIOE, DRIVE_START_BUTTON_PIN,  PAL_MODE_INPUT_PULLDOWN ); // enable button
  palSetPadMode( GPIOE, DRIVE_DIR_BUTTON_PIN,    PAL_MODE_INPUT_PULLDOWN ); // direction button
  palSetPadMode( GPIOE, DRIVE_ENABLE_BUTTON_PIN, PAL_MODE_INPUT_PULLDOWN ); // enable button

  /* Set up hardware pin mode as digital output  */
  palSetPadMode( GPIOE, CONTROL_DRIVE_DIR_PIN,    PAL_MODE_OUTPUT_PUSHPULL ); // direction output (control signal)
  palSetPadMode( GPIOF, CONTROL_DRIVE_ENABLE_PIN, PAL_MODE_OUTPUT_PUSHPULL ); // enable output (control signal)


#endif

  while (1)
  {
      Ain = commonADC1UnitGetValue ( 1 );
      //duty_cucle = (float)Ain * 10000.0 / 4096.0;
      //period_s = (  Ain * 1000 / 4096 );
      //pwmChangePeriod(&PWMD1, 5000);

      motor_run_enable =  ifDriverEnable ();



#if 1
      if ( motor_run_enable )
      {
          pwmEnableChannel( &PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH (&PWMD1,5000) );
      }

      else
      {
          pwmEnableChannel( &PWMD1, 0, 0 );
      }

#endif

      //chprintf( (BaseSequentialStream *)&SD7, "ADC:  %d\t En %d\t DirR %d\t DirL %d\t Duty %d\n\r",
           //     Ain, drive_enable, right_dir, left_dir, *sd_value   );


      /* read data from serial */
      //rcv_data = sdGetTimeout( &SD7, TIME_IMMEDIATE );
      int test = sdReadTimeout( &SD3, sd_buff, 4, TIME_IMMEDIATE   );
      sd_value = &sd_buff[3];

      uint8_t *sign;
      chprintf( (BaseSequentialStream *)&SD3, "%d\n\r",
                     *sd_value   );





      chThdSleepMilliseconds( 10 );
  }
}

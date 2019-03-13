#include <lld_control.h>

/* BUTTONS*/
#define DRIVE_START_BUTTON_PORT       GPIOE
#define DRIVE_START_BUTTON_PIN        4

#define DRIVE_DIR_BUTTON_PORT         GPIOE
#define DRIVE_DIR_BUTTON_PIN          5

#define DRIVE_ENABLE_BUTTON_PORT      GPIOE
#define DRIVE_ENABLE_BUTTON_PIN       6


/*Control pins*/
#define CONTROL_DRIVE_ENABLE_PORT     GPIOF
#define CONTROL_DRIVE_ENABLE_PIN      14

#define CONTROL_DRIVE_DIR_PORT        GPIOE
#define CONTROL_DRIVE_DIR_PIN         11

/* Variables */
static volatile bool right_dir = 0, left_dir = 0, drive_enable = 0, drive_start = 0;





/********************************/
/*********  PWM Unit    *********/
/********************************/

#define PWM_FREQ           1000000  /* PWM clock frequency [Hz] */
#define PWM_PERIOD         100      /* PWM period[tics]  */

PWMConfig pwm1conf = {
    /* 10kHz PWM clock frequency.   */
    .frequency = PWM_FREQ,

    /* PWM period (ticks). PWM_period[s] = PWM_period[ticks]/PWM_frequency */
    .period    = PWM_PERIOD,
    .callback  = NULL,
    .channels  = {
                  {.mode = PWM_OUTPUT_ACTIVE_HIGH, .callback = NULL}, // Channel 1 is working (CH1 = PE9)
                  {.mode = PWM_OUTPUT_DISABLED,    .callback = NULL},
                  {.mode = PWM_OUTPUT_DISABLED,    .callback = NULL},
                  {.mode = PWM_OUTPUT_DISABLED,    .callback = NULL}
                  },
    .cr2        = 0,
    .dier       = 0
};


void PWMUnitInit ( void )
{
    palSetLineMode( PAL_LINE( GPIOE, 9 ),  PAL_MODE_ALTERNATE(1) );
    pwmStart( &PWMD1, &pwm1conf );
}


/* EXT Driver callback
 * Motor driver control ENABLE button change state:
 * (0->1: drive enable)
 * (1->0: drive disable)
 */
static void extcb_en_button( EXTDriver *extp, expchannel_t channel )
{
    /* to avoid warnings */
    (void)extp;
    (void)channel;

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
static void extcb_start_button( EXTDriver *extp, expchannel_t channel )
{
    /* to avoid warnings */
    (void)extp;
    (void)channel;

    palToggleLine(LINE_LED3);

    if ( palReadPad ( DRIVE_START_BUTTON_PORT, DRIVE_START_BUTTON_PIN ) )
    {
        drive_start = 1;
        palSetLine(LINE_LED3);
    }
    else
    {
        drive_start = 0;
        palClearLine(LINE_LED3);
    }

}

/* Driver DIRECTION toggle button changing state occure:
 * (0->1: right direction)
 * (1->0: left direction )
 */
static void extcb_dir_button( EXTDriver *extp, expchannel_t channel )
{
    /* to avoid warnings */
    (void)extp;
    (void)channel;

    if ( palReadPad (DRIVE_DIR_BUTTON_PORT, DRIVE_DIR_BUTTON_PIN) )
    {
        left_dir = 0;
        right_dir = 1;
        palClearLine( LINE_LED2 );

        /* to control driver's "direction" pin */
        palSetPad ( CONTROL_DRIVE_DIR_PORT, CONTROL_DRIVE_DIR_PIN );

    }
    else
    {
        right_dir = 0;
        left_dir = 1;
        palSetLine( LINE_LED2 );

        /* to control driver's "direction" pin */
        palClearPad (  CONTROL_DRIVE_DIR_PORT, CONTROL_DRIVE_DIR_PIN );
    }
}

static bool         lld_control_Initialized       = false;

/**
 * @brief   Initialize periphery that used for control motor
 * @note    Stable for repeated calls
 */
void lldControlInit ( void )
{
      if ( lld_control_Initialized )
                return;

      /*EXT driver initialization*/
      commonExtDriverInit();

      /* Define channel config structure */
      EXTChannelConfig en_b_ch_conf, start_b_ch_conf, dir_b_ch_conf;

      /* Fill in configuration for channel */
      en_b_ch_conf.mode  = EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOE;
      en_b_ch_conf.cb    = extcb_en_button;

      start_b_ch_conf.mode  = EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOE;
      start_b_ch_conf.cb    = extcb_start_button;

      dir_b_ch_conf.mode  = EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOE;
      dir_b_ch_conf.cb    = extcb_dir_button;

      /* Set up EXT channels hardware pin mode as digital input.
       * Needs to be set up before set EXT channel mode! */
      palSetPadMode( GPIOE, DRIVE_START_BUTTON_PIN,  PAL_MODE_INPUT_PULLDOWN ); // enable button
      palSetPadMode( GPIOE, DRIVE_ENABLE_BUTTON_PIN, PAL_MODE_INPUT_PULLDOWN ); // enable button
      palSetPadMode( GPIOE, DRIVE_DIR_BUTTON_PIN,    PAL_MODE_INPUT_PULLDOWN ); // direction button

      /* Set channel (second arg) mode with filled configuration */
      extSetChannelMode( &EXTD1, DRIVE_ENABLE_BUTTON_PIN, &en_b_ch_conf    );
      extSetChannelMode( &EXTD1, DRIVE_START_BUTTON_PIN,  &start_b_ch_conf );
      extSetChannelMode( &EXTD1, DRIVE_DIR_BUTTON_PIN,    &dir_b_ch_conf   );


      /* Set up hardware pin mode as digital output  */
      palSetPadMode( GPIOE, CONTROL_DRIVE_DIR_PIN,    PAL_MODE_OUTPUT_PUSHPULL ); // direction output (control signal)
      palSetPadMode( GPIOF, CONTROL_DRIVE_ENABLE_PIN, PAL_MODE_OUTPUT_PUSHPULL ); // enable output (control signal)

      /* PWM Unit initialization */
      PWMUnitInit();

      /* Set initialization flag */
      lld_control_Initialized = true;
}

bool ifDriverEnable ( void )
{
#if 0
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

    //&& drive_enable
    return palReadPad ( DRIVE_START_BUTTON_PORT, DRIVE_START_BUTTON_PIN );

#endif

    if ( drive_start  )                return 1;
    else                               return 0;
}

void motorRun ( void )
{
    bool motor_run_enable = 0;

    motor_run_enable   =  ifDriverEnable ();
    if ( motor_run_enable )
    {
        pwmEnableChannel( &PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH ( &PWMD1, 5000 ) );
    }
    else
    {
        pwmEnableChannel( &PWMD1, 0, 0 );
    }
}

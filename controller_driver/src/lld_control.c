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
static bool right_dir = 0, left_dir = 0, drive_enable = 0, drive_start = 0;





/********************************/
/*********  PWM Unit    *********/
/********************************/

#define PWM_FREQ           1000000  /* PWM clock frequency [Hz] */
#define PWM_PERIOD         100      /* PWM period[tics]  */

//#define DUTY_CYCLE         50       /* % of period */
//static const uint16_t pwm_period_ticks =  PWM_PERIOD_S * PWM_FREQ;
//const uint8_t  duty_cycle_ticks =  DUTY_CYCLE * PWM_PERIOD / 100;



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
    //pwmEnableChannel( &PWMD1, 0, duty_cycle_ticks );

}



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


void lldControlInit ( void )
{
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

      PWMUnitInit();
}

bool ifDriverEnable ( void )
{
    //&& drive_enable
    if ( drive_start  )                return 1;
    else                               return 0;
}


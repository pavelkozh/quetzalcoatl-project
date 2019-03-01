#include <lld_control.h>



/********************************/
/*********  PWM Unit    *********/
/********************************/


#define PWM_FREQ           1000000  /* PWM clock frequency [Hz] */
#define PWM_PERIOD         50      /* PWM period[tics]  */

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

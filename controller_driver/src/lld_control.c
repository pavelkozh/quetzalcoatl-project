#include <lld_control.h>
#include <math.h>

/* @brief: starts PWM Driver.
 */
void PWMUnitInit ( void );

/* @brief: starts GPT Driver.
 */
void GPT5Init ( void );


/* Variables */
static volatile bool  drive_dir = 0, drive_enable = 0, drive_start = 0;
static  uint32_t pwm_pulses_counter = 0;
static bool drive_state = 0; //0 - drive stoped, 1 - drive run


/**************************/
/*********  GPT ***********/
/**************************/
static void gpt_overflow_cb ( GPTDriver *timeIntervalsDriver );
static GPTDriver  *timeIntervalsDriver = DRIVE_GPTD;
static const GPTConfig timeIntervalsCfg = {
     .frequency      =  STM32_TIMCLK1,      // PSC must be 0 for Slave mode
     .callback       =  gpt_overflow_cb,
     .cr2            =  0,
     .dier           =  TIM_DIER_UIE
};


/***************************/
/*********  PWM  ***********/
/***************************/
uint32_t cb_pwm_pulses_counter = 0;

static void pwmc1_counter_reset_cb ( PWMDriver *pwmp )
{
    (void) pwmp;
//    if (drive_state)
//    {
//        cb_pwm_pulses_counter += 1;
//        driveSetSpeed ( (int16_t) (10000 - 8750*(powf(((float)cb_pwm_pulses_counter / DRIVE_PWM_IMPULSE_QUANTITY ),1)))  );
//        //driveSetSpeed ( (sint16_t) (10000 - 9000*(((float)cb_pwm_pulses_counter / IMPULSE_QUANTITY )*((float)cb_pwm_pulses_counter / IMPULSE_QUANTITY ) *((float)cb_pwm_pulses_counter / IMPULSE_QUANTITY )))  );
//        if ( cb_pwm_pulses_counter >= DRIVE_PWM_IMPULSE_QUANTITY ) cb_pwm_pulses_counter = 0;
//    }


}

PWMConfig pwm1conf = {

    .frequency = DRIVE_PWM_FREQ,

    /* PWM period (ticks). PWM_period[s] = PWM_period[ticks]/PWM_frequency */
    .period    = DRIVE_PWM_PERIOD,
    .callback  = NULL,
    .channels  = {
                  {.mode = PWM_OUTPUT_ACTIVE_HIGH, .callback = NULL}, // Channel 1 is working (CH1 = PC6)
                  {.mode = PWM_OUTPUT_DISABLED,    .callback = NULL},
                  {.mode = PWM_OUTPUT_DISABLED,    .callback = NULL},
                  {.mode = PWM_OUTPUT_DISABLED,    .callback = NULL}
                  },
    .cr2        = TIM_CR2_MMS_2, //Master Mode OC1REF -> TRGO
    .dier       = 0
};

void PWMUnitInit ( void )
{
    palSetLineMode( PAL_LINE( DRIVE_PWM_PORT, DRIVE_PWM_PIN),  PAL_MODE_ALTERNATE(DRIVE_PWM_PAL_MODE_ALTERNATE) );
    pwmStart( DRIVE_PWMD, &pwm1conf );
    pwmEnablePeriodicNotification(DRIVE_PWMD);

}

//Init Slave Timer
void GPT5Init(void ){
    gptStart( timeIntervalsDriver, &timeIntervalsCfg );
    timeIntervalsDriver -> tim->PSC = 0;
    timeIntervalsDriver -> tim -> SMCR |= DRIVE_GPTD_TIM_REG_SMCR ;
}


/* GPT Driver callback*/
static void gpt_overflow_cb ( GPTDriver *timeIntervalsDriver ){
  (void)timeIntervalsDriver;
  pwmDisableChannel(DRIVE_PWMD,0);
  palClearPad(GPIOE, 5);
  drive_state = 0;
}

static void limit_switch_1_cb( EXTDriver *extp, expchannel_t channel ){
   (void) extp ;
   (void) channel ;

}
static void limit_switch_2_cb( EXTDriver *extp, expchannel_t channel ){
  (void) extp ;
  (void) channel ;
}

static bool         lld_control_Initialized       = false;

/**
 * @brief   Initialize periphery that used for control motor
 * @note    Stable for repeated calls
 */
void drivelldControlInit ( void ){
      if ( lld_control_Initialized ) return;

      /*Setting pins*/
      palSetLineMode( PAL_LINE(DRIVE_DIRECTION_PIN_PORT,DRIVE_DIRECTION_PIN), PAL_MODE_OUTPUT_PUSHPULL);
      palSetPadMode( GPIOE,5, PAL_MODE_OUTPUT_PUSHPULL);
#if DRIVE_LIMIT_SWITCH_USE
      /*EXT driver initialization*/
      commonExtDriverInit();

      EXTChannelConfig limit_switch_1_conf,limit_switch_2_conf;

      limit_switch_1_conf.mode  = EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | DRIVE_LIMIT_SWITCH_1_EXT_MODE_GPIO;
      limit_switch_1_conf.cb    = limit_switch_1_cb;

      limit_switch_2_conf.mode  = EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | DRIVE_LIMIT_SWITCH_1_EXT_MODE_GPIO;
      limit_switch_2_conf.cb    = limit_switch_2_cb;

      palSetLineMode( PAL_LINE(DRIVE_LIMIT_SWITCH_1_PIN_PORT,DRIVE_LIMIT_SWITCH_1_PIN), PAL_MODE_INPUT_PULLUP);
      palSetLineMode( PAL_LINE(DRIVE_LIMIT_SWITCH_2_PIN_PORT,DRIVE_LIMIT_SWITCH_2_PIN), PAL_MODE_INPUT_PULLUP);

      extSetChannelMode( &EXTD1, DRIVE_LIMIT_SWITCH_1_PIN, &limit_switch_1_conf );
      extSetChannelMode( &EXTD1, DRIVE_LIMIT_SWITCH_2_PIN, &limit_switch_2_conf );
#endif
      /*GPT init*/
      GPT5Init();
      /* PWM Unit initialization */
      PWMUnitInit();
      /* Set initialization flag */

      lld_control_Initialized = true;
}

/*
 * @brief :  drive run
 * @params:  dir - direction rotation
 *           ang - angle in pusle (pulse per revolution)
 *           speed
 */
void driveRun (bool dir, uint32_t ang, uint16_t speed){

  if(dir != NULL) driveSetDirection(dir);
  if(speed != NULL) driveSetSpeed(speed);

  if(ang != 0){
    if(ang != NULL)gptStartOneShot(timeIntervalsDriver, ang-1);
    driveresetPwmCounter ();
    pwmEnableChannel( DRIVE_PWMD, 0, PWM_PERCENTAGE_TO_WIDTH ( DRIVE_PWMD, 5000 ) );
    //gpt1_overflow_counter = 0;
    //gptStartContinuous(timeIntervalsDriver1 ,  Timer1Period);
    palSetPad(GPIOE, 5);
  }
  drive_state = 1;
}

/*
 * @brief :  drive stop
 */
void driveStop(){
  pwmDisableChannel(DRIVE_PWMD,0);
  gptStopTimer(timeIntervalsDriver);
  drive_state = 0;
}


/*
 * @brief :  set motor's rotation direction
 * @params:  dir - direction rotation
 */
void driveSetDirection ( bool dir ){

  #if DRIVE_INVERT_ROTATION_DIRECTION
      palWriteLine(PAL_LINE(DRIVE_DIRECTION_PIN_PORT,DRIVE_DIRECTION_PIN), !dir);
  #else
      palWriteLine(PAL_LINE(DRIVE_DIRECTION_PIN_PORT,DRIVE_DIRECTION_PIN), dir);
  #endif

}

/*
 * @brief :  set motor's rotation speed
 * @params:  speed - speed of rotation (max speed - 100kHz = 1500rpm)
 */
uint16_t driveSetSpeed(uint16_t speed){
//  if(speed >100 ) speed =100;
//  speed = (uint16_t)( 65534 - speed*(65534- DRIVE_MIN_PULSE_WIDTH )/100 );
  pwmChangePeriodI(DRIVE_PWMD,speed);
  if(drive_state) pwmEnableChannel( DRIVE_PWMD, 0, PWM_PERCENTAGE_TO_WIDTH ( DRIVE_PWMD, 5000 ) );
  return speed;
}


uint32_t drivegetPwmCounter ( void ){
    return cb_pwm_pulses_counter;
}

void driveresetPwmCounter ( void ){
    cb_pwm_pulses_counter = 0;
}


bool driveState(){
  return drive_state;
}
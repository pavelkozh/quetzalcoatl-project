#include <lld_control.h>


//PWM define
#define DRIVE_PWM_FREQ            50000000   // PWM clock frequency [Hz]
#define DRIVE_PWM_PERIOD          500       // PWM period[tics]
#define DRIVE_PWM_PULSE_WIDTH     100       // PWM pulse width

#define DRIVE_GPTD_TIM_REG_SMCR     TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1 | TIM_SMCR_SMS_2 | TIM_SMCR_TS_0 //Settings for Slave mode PWMD

//Drive
#define DRIVE_INVERT_ROTATION_DIRECTION   FALSE   // Invert rotation direction 
#define DRIVE_DIRECTION_PIN         8     // Pin of set directions 
#define DRIVE_DIRECTION_PIN_PORT      GPIOB   // Pin port set directions


//Limit Switch
#define DRIVE_LIMIT_SWITCH_1_PIN            9
#define DRIVE_LIMIT_SWITCH_1_PIN_PORT       GPIOB
#define DRIVE_LIMIT_SWITCH_1_EXT_MODE_GPIO  EXT_MODE_GPIOB
#define DRIVE_LIMIT_SWITCH_2_PIN            15
#define DRIVE_LIMIT_SWITCH_2_PIN_PORT       GPIOB
#define DRIVE_LIMIT_SWITCH_2_EXT_MODE_GPIO  EXT_MODE_GPIOB


// Motor define
#define DRIVE_MAX_ANG                   160000      // 40 revolution or 20cm (5mm per revolution)
#define DRIVE_MAX_SPEED                 100000      // 100Khz or 1500 rpm
#define DRIVE_MIN_PULSE_WIDTH           (uint32_t)DRIVE_PWM_FREQ / DRIVE_MAX_SPEED

//Desired pwm impulses quantity (send to driver)
#define DRIVE_PWM_IMPULSE_QUANTITY    17 // 200 for stepper motor


/* @brief: starts PWM Driver.
 */
void PWMUnitInit ( MotorDriver *mtd );
/* Variables */
// static volatile bool  drive_dir = 0, drive_enable = 0, drive_start = 0;
// static  uint32_t pwm_pulses_counter = 0;
// static bool drive_state = 0; //0 - drive stoped, 1 - drive run


/***************************/
/*********  PWM  ***********/
/***************************/
uint8_t pwm_conf_cnt = 0;
PWMConfig pwm_conf_arr[14];

PWMConfig pwmconf = {

        .frequency = DRIVE_PWM_FREQ,

        /* PWM period (ticks). PWM_period[s] = PWM_period[ticks]/PWM_frequency */
        .period    = DRIVE_PWM_PERIOD,
        .callback  = NULL,
        .channels  = {
                        {.mode = PWM_OUTPUT_ACTIVE_HIGH, .callback = NULL},
                        {.mode = PWM_OUTPUT_DISABLED,    .callback = NULL},
                        {.mode = PWM_OUTPUT_DISABLED,    .callback = NULL},
                        {.mode = PWM_OUTPUT_DISABLED,    .callback = NULL}
                    },
        .cr2        = 0, //TIM_CR2_MMS_2, Master Mode OC1REF -> TRGO
        .dier       = 0
};

/*
* @brief:  Rising callcallback for all motor driver.
* @note:  It need to include in user callback
* @param: mtd - Motor Driver
*/
void risingEdgeCb(MotorDriver *mtd){
    return;
}

/*
* @brief:  Falling callback for all motor driver.
* @note:   It need to include in user callback
* @param:  mtd - Motor Driver
*/
void fallingEdgeCb(MotorDriver *mtd){


    switch(mtd->mode){

        case MOTOR_MODE_TRACKING: 

                if(mtd -> tracked_position > mtd -> max_position) mtd -> tracked_position = mtd -> max_position;
                if(mtd -> tracked_position <  mtd -> min_position) mtd -> tracked_position = mtd -> min_position;

                if( mtd->position > mtd -> tracked_position )
                    { MotorSetDirection(mtd,1); pwmEnableChannel( mtd -> pwmd, 0, DRIVE_PWM_PULSE_WIDTH); mtd -> state = MOTOR_MOVING; }
                if( mtd->position < mtd -> tracked_position )
                    { MotorSetDirection(mtd,0); pwmEnableChannel( mtd -> pwmd, 0, DRIVE_PWM_PULSE_WIDTH); mtd -> state = MOTOR_MOVING; }

                if(mtd->position == mtd -> tracked_position){
                    pwmEnableChannel( mtd -> pwmd, 0, 0);
                    mtd -> state = MOTOR_STOPED;
                }else{

                if( palReadLine(mtd->dir_line) ) //
                    mtd -> position--;
                else
                    mtd -> position++;
            }
                break;

            break;

        case MOTOR_MODE_CONTINUOUS:
            if( palReadLine(mtd->dir_line) )
                mtd -> position--;
            else
                mtd -> position++;
            if( (mtd -> position  >=  mtd -> max_position) || (mtd -> position  <= mtd -> min_position) ){

                MotorStop( mtd) ;

            }
            break;

        case MOTOR_MODE_CALIBRATION:
            if(mtd->position == mtd -> tracked_position){
                MotorStop( mtd);
                MotorResetPotision(mtd); 
            }
            else{
                if( mtd->position > mtd -> tracked_position )     { pwmEnableChannel( mtd -> pwmd, 0, DRIVE_PWM_PULSE_WIDTH); mtd -> state = MOTOR_MOVING;}
                if( mtd->position < mtd -> tracked_position )     { pwmEnableChannel( mtd -> pwmd, 0, DRIVE_PWM_PULSE_WIDTH); mtd -> state = MOTOR_MOVING;}
                if( palReadLine(mtd->dir_line) ) 
                    mtd -> position--;
                else
                    mtd -> position++;
            }
        break;

        default:

            if( (mtd -> position  >=  mtd -> max_position) || (mtd -> position  <= mtd -> min_position) )  MotorStop( mtd); 

            break;
    }

}


void PWMUnitInit ( MotorDriver *mtd ){
        pwm_conf_arr[ pwm_conf_cnt ] = pwmconf;
        pwm_conf_arr[ pwm_conf_cnt ].callback = mtd -> rising_edge_cb;
        pwm_conf_arr[ pwm_conf_cnt ].channels[0].callback = mtd -> falling_edge_cb;
        pwmStart( mtd -> pwmd , &pwm_conf_arr[ pwm_conf_cnt++ ]);
        // pwmEnablePeriodicNotification(mtd->pwmd);
        // pwmEnableChannelNotification(mtd->pwmd,0);
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
void MotorlldControlInit ( MotorDriver *mtd ){
            //if ( lld_control_Initialized ) return;

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

            mtd -> state = MOTOR_STOPED;
            mtd -> position = 0;
            mtd -> tracked_position = 0;

            /* PWM Unit initialization */
            PWMUnitInit( mtd);
            /* Set initialization flag */

            //-------------START SETTINGS!------------//


            //lld_control_Initialized = true;
}

/*
 * @brief :  drive run in continuous mode
 * @params:  mtd - Motor Driver
 *           dir - direction rotation
 *           speed - speed of rotutuion in ticks of pwm timer
 */
void MotorRunContinuous( MotorDriver *mtd, bool dir, uint16_t speed){

    mtd -> mode = MOTOR_MODE_CONTINUOUS;
    if(dir == 0 && mtd -> position >= mtd -> max_position)  return;
    if(dir == 1 && mtd -> position <= mtd -> min_position)  return;
    MotorSetDirection( mtd , dir);
    MotorSetSpeed( mtd, speed);
    pwmEnableChannel( mtd -> pwmd, 0, DRIVE_PWM_PULSE_WIDTH);
    pwmEnablePeriodicNotification(mtd->pwmd);
    pwmEnableChannelNotification(mtd->pwmd,0);
    mtd -> state = MOTOR_MOVING;

}

/*
 * @brief :  drive run in tracking mode
 * @params:  mtd - Motor Driver
 *           speed - speed of rotutuion in ticks of pwm timer
 */

void MotorRunTracking(MotorDriver *mtd, uint16_t speed){
    bool dir = 0;

    if(mtd->mode != MOTOR_MODE_TRACKING)  mtd -> tracked_position = mtd -> position;
        mtd -> mode = MOTOR_MODE_TRACKING;
    if(mtd -> tracked_position > mtd -> max_position) mtd -> tracked_position = mtd -> max_position;
    if(mtd -> tracked_position < mtd -> min_position) mtd -> tracked_position = mtd -> min_position;
    // if( (mtd -> position - mtd -> tracked_position) > 0)  dir = 1;
    // if( (mtd -> position - mtd -> tracked_position) < 0)  dir = 0;

    // if(dir == 0 && mtd -> position >= mtd -> max_position)  return;
    // if(dir == 1 && mtd -> position <= 0)                    return;
    MotorSetDirection( mtd , dir);
    MotorSetSpeed( mtd, speed);
    pwmEnableChannel( mtd -> pwmd, 0, DRIVE_PWM_PULSE_WIDTH);
    pwmEnablePeriodicNotification(mtd->pwmd);
    pwmEnableChannelNotification(mtd->pwmd,0);
    mtd -> state = MOTOR_MOVING;

}

/*
 * @brief :  drive run in tracking mode
 * @params:  mtd - Motor Driver
 *           dir - direction rotation
 *           speed - speed of rotutuion in ticks of pwm timer
 *           pulses - impulses of the position to be shifted
 */
void MotorRunCaclibration( MotorDriver *mtd, bool dir, uint16_t speed, uint32_t pulses){

    MotorSetDirection( mtd , dir);
    MotorSetSpeed( mtd, speed);

    if(mtd->mode != MOTOR_MODE_CALIBRATION)  mtd -> tracked_position = mtd -> position;
    mtd -> mode = MOTOR_MODE_CALIBRATION;

    //mtd -> position = 0;
    if(dir)
        mtd -> tracked_position -= pulses;
    else
        mtd -> tracked_position += pulses;

    pwmEnableChannel( mtd -> pwmd, 0, DRIVE_PWM_PULSE_WIDTH);
    pwmEnablePeriodicNotification(mtd->pwmd);
    pwmEnableChannelNotification(mtd->pwmd,0);
    mtd -> state = MOTOR_MOVING;
}

/*
 * @brief :  drive stop
 * @params:  mtd - Motor Driver
 */
void MotorStop( MotorDriver *mtd ){

    pwmDisableChannel(mtd->pwmd,0);
    pwmDisablePeriodicNotification(mtd->pwmd);
    pwmDisableChannelNotification(mtd->pwmd,0);
    mtd -> state = MOTOR_STOPED;
}


/*
 * @brief :  set motor's rotation direction
 * @params:  mtd - Motor Driver
 *           dir - direction rotation
 */
void MotorSetDirection( MotorDriver *mtd, bool dir ){

            palWriteLine(mtd->dir_line, dir);
}

/*
 * @brief :  set motor's rotation speed
 * @params:  speed - speed of rotation (max speed = 300 ticks of pwm timer)
 */
void MotorSetSpeed( MotorDriver *mtd, uint16_t speed ){
//  if(speed >100 ) speed =100;
//  speed = (uint16_t)( 65534 - speed*(65534- DRIVE_MIN_PULSE_WIDTH )/100 );

    if(speed < DRIVE_PWM_PULSE_WIDTH * 2) speed =DRIVE_PWM_PULSE_WIDTH * 2;
    pwmChangePeriod( mtd -> pwmd, speed );
}


void MotorResetPotision ( MotorDriver *mtd ){
    mtd -> position = 0;
    mtd -> tracked_position = 0;
}


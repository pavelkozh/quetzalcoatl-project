#include <pedals.h>
#include <lld_control.h>
#include <lld_ext_dac.h>




/****************************************************************************/
/************COMMON FOR BRAKE AND CLUTCH PEDALS MOTOR CONTROL SETUP *********/
/****************************************************************************/


/* encoder ticks units 4000 ticks = 1 rev ( or 5mm translational movement) */



/* Service callback functions declaration*/
void risingEdgeClutchMCallback(PWMDriver *pwmd);
void fallingEdgeClutchMCallback(PWMDriver *pwmd);
void risingEdgeBreakMCallback(PWMDriver *pwmd);
void fallingEdgeBreakMCallback(PWMDriver *pwmd);

/* Clutch motor structure declaration*/
MotorDriver ClutchM = {
    .pwmd            =   &PWMD3,
    .dir_line        =   PAL_LINE(GPIOB, 8),
    .rising_edge_cb  =   risingEdgeClutchMCallback,
    .falling_edge_cb =   fallingEdgeClutchMCallback,
    .max_position    =   100000,
    .min_position     =  0
};
void risingEdgeClutchMCallback(PWMDriver *pwmd){

    (void) pwmd;
    risingEdgeCb(&ClutchM);
}
void fallingEdgeClutchMCallback(PWMDriver *pwmd){

    (void) pwmd;
    fallingEdgeCb(&ClutchM);
}


/* Brake motor structure declaration*/
MotorDriver BreakM = {
    .pwmd            =   &PWMD4,
    .dir_line        =   PAL_LINE(GPIOD, 11),
    .rising_edge_cb  =   risingEdgeBreakMCallback,
    .falling_edge_cb =   fallingEdgeBreakMCallback,
    .max_position    =   40000,
    .min_position     =  0
};

void risingEdgeBreakMCallback(PWMDriver *pwmd){

    (void) pwmd;
    risingEdgeCb(&BreakM);
}
void fallingEdgeBreakMCallback(PWMDriver *pwmd){

    (void) pwmd;
    fallingEdgeCb(&BreakM);
}

static bool if_pedals_module_initialized = 0;

/*
 * @brief      Perephery setup and lld initialization
 */
void pedalsInit ( void ){
     if ( if_pedals_module_initialized )
     {
         return;
     }

     /* Clutch init*/
     palSetLineMode( PAL_LINE( GPIOC, 6),  PAL_MODE_ALTERNATE(2) );
     palSetLineMode( ClutchM.dir_line, PAL_MODE_OUTPUT_PUSHPULL);
     MotorlldControlInit( &ClutchM );

     /* Brake init*/
     palSetLineMode( PAL_LINE( GPIOD, 12),  PAL_MODE_ALTERNATE(2) );
     palSetLineMode( BreakM.dir_line, PAL_MODE_OUTPUT_PUSHPULL);
     MotorlldControlInit( &BreakM );

     /* Accelerator pedal init*/
     extDacInit();
     if_pedals_module_initialized = 1;
}


/****************************************************************************/
/*********************** CLUTCH PEDAL MOTOR CONTROL *************************/
/****************************************************************************/

/*
 * @brief     Move clutch pedal to target position
 * @params    position [0...CLUTCH_MAX_POS] (CLUTCH_MAX_POS - define in pedals.h)
 *              -> 0.0 corresponds to fully released pedal
 *              -> CLUTCH_MAX_POS corresponds to fully pressed pedal
 *            speed [X...500]    -> movement speed in PWM period. *
 *              -> 500 is a max speed
 *              -> min speed have not boundary
 * @note      speed(rpm) = PWM frequency/PWM period/(quantity of encoder impulses per revolution * 4)*60
 * @return    current position
 */
int32_t pedalsClutchMove (double position, uint16_t speed ){
    MotorRunTracking( &ClutchM,   speed);
    ClutchM.tracked_position = position;
    return  ClutchM.position;
}

/*
 * @brief     Fully press clutch pedal
 * @params    speed - movement speed (look "moveClutchPedal" function description )
 */
void pedalsClutchPress ( uint16_t speed ){
    MotorRunContinuous(&ClutchM, 0, speed);
}

/*
 * @brief     Fully release clutch pedal
 * @params    speed - movement speed (look "moveClutchPedal" function description )
 */
void pedalsClutchRelease ( uint16_t speed ){
    MotorRunContinuous(&ClutchM, 1, speed);
}

/* Service function */
void pedalsClutchCalibrate( bool dir, uint16_t speed, uint16_t step ){
    MotorRunCaclibration( &ClutchM, dir, speed, step );
}

/*
 * @brief     stops clutch pedal motor
 */
void pedalsClutchStop ( void ){
    MotorStop( &ClutchM );
}

/*
 * @brief     Function can change motor speed  "on the fly"
 * @params    new_speed (look "moveClutchPedal" function description for units )
 */
void pedalsClutchChangeSpeed ( uint16_t new_speed ){
    MotorSetSpeed( &ClutchM, new_speed );
}

/*******************************/
/* @brief       Get additional information about clutch pedal motor
 * @note        This is not a feedback information,
 *              but a values which microcontroller was send to driver
 * @return      position [0...BRAKE_MAX_POS] (BRAKE_MAX_POS - define in pedals.h)
 *                  -> 0.0 corresponds to fully released pedal
 *                  -> BRAKE_MAX_POS corresponds to fully pressed pedal
 */
int32_t pedalsClutchGetPosition ( void ){
    return ClutchM.position;
}

/*
 * @brief        Get additional information about clutch pedal motor
 * @note         This is not a feedback information,
 *               but a values which microcontroller was send to driver
 * @return       1 - moving  //  0 - stop
 */
bool pedalsClutchGetState ( void ){
    return ClutchM.state;
}

/*
 * @brief        Get additional information about clutch pedal motor
 * @note         This is not a feedback information,
 *               but a values which microcontroller was send to driver
 * @return       2 - MOTOR_MODE_SHOT
 *               3 - MOTOR_MODE_TRACKING
 *               4 - MOTOR_MODE_CONTINUOUS
 *               5 - MOTOR_MODE_CALIBRATION
 */
uint8_t pedalsClutchGetMode ( void ){
    return ClutchM.mode;
}

/* @brief        Get additional information about clutch pedal motor
 * @note         This is not a feedback information,
 *               but a values which microcontroller was send to driver
 * @return       speed [X...500]    -> movement speed in PWM period. *
 *                 -> 500 is a max speed
 *                 -> min speed have not boundary
 */
uint16_t pedalsClutchGetSpeed ( void ){
    return  ClutchM.pwmd->period;
}

int32_t pedalsClutchGetMaxPosition ( void )
{
    return ClutchM.max_position;
}

int32_t pedalsClutchGetMinPosition ( void )
{
    return ClutchM.min_position;
}
/*******************************/



/****************************************************************************/
/*********************** BRAKE PEDAL MOTOR CONTROL **************************/
/****************************************************************************/

/*
 * @brief     Move Brake pedal to target position
 * @params    position [0...BRAKE_MAX_POS] (BRAKE_MAX_POS - define in pedals.h)
 *              -> 0.0 corresponds to fully released pedal
 *              -> BRAKE_MAX_POS corresponds to fully pressed pedal
 *            speed [X...500]    -> movement speed in PWM period. *
 *              -> 500 is a max speed
 *              -> min speed have not boundary
 * @note      speed(rpm) = PWM frequency/PWM period/(quantity of encoder impulses per revolution * 4)*60
 * @return    current position
 */
int32_t pedalsBrakeMove (double position, uint16_t speed ){
    MotorRunTracking( &BreakM,   speed);
    BreakM.tracked_position = position;
    return BreakM.position;
}

/*
 * @brief     Fully press Brake pedal
 * @params    speed - movement speed (look "moveBrakePedal" function description )
 */
void pedalsBrakePress ( uint16_t speed ){
    MotorRunContinuous(&BreakM, 0, speed);
}

/*
 * @brief     Fully release Brake pedal
 * @params    speed - movement speed (look "moveBrakePedal" function description )
 */
void pedalsBrakeRelease( uint16_t speed ){
    MotorRunContinuous(&BreakM, 1, speed);
}

/* Service function */
void pedalsBrakeCalibrate ( bool dir, uint16_t speed, uint16_t step ){
    MotorRunCaclibration( &BreakM, dir, speed, step );
}

/*
 * @brief     stops Brake pedal motor
 */
void pedalsBrakeStop ( void ){
    MotorStop( &BreakM );
}

/*
 * @brief     Function can change motor speed  "on the fly"
 * @params    new_speed (look "moveClutchPedal" function description for units )
 */
void pedalsBrakeChangeSpeed ( uint16_t new_speed ){
    MotorSetSpeed( &BreakM, new_speed );
}


/* @brief       Get additional information about brake pedal motor
 * @note        This is not a feedback information,
 *              but a values which microcontroller was send to driver
 * @return      position [0...BRAKE_MAX_POS] (BRAKE_MAX_POS - define in pedals.h)
 *                  -> 0.0 corresponds to fully released pedal
 *                  -> BRAKE_MAX_POS corresponds to fully pressed pedal
 */
int32_t pedalsBrakeGetPosition ( void ){
    return BreakM.position;
}

/*
 * @brief        Get additional information about brake pedal motor
 * @note         This is not a feedback information,
 *               but a values which microcontroller was send to driver
 * @return       1 - moving  //  0 - stop
 */
bool pedalsBrakeGetState ( void ){
    return BreakM.state;
}

/*
 * @brief        Get additional information about brake pedal motor
 * @note         This is not a feedback information,
 *               but a values which microcontroller was send to driver
 * @return       2 - MOTOR_MODE_SHOT
 *               3 - MOTOR_MODE_TRACKING
 *               4 - MOTOR_MODE_CONTINUOUS
 *               5 - MOTOR_MODE_CALIBRATION
 */
uint8_t pedalsBrakeGetMode ( void ){
    return BreakM.mode;
}

/* @brief        Get additional information about brake pedal motor
 * @note         This is not a feedback information,
 *               but a values which microcontroller was send to driver
 * @return       speed [X...500]    -> movement speed in PWM period. *
 *                 -> 500 is a max speed
 *                 -> min speed have not boundary
 */
uint16_t pedalsBrakeGetSpeed ( void ){
    return  BreakM.pwmd->period;
}

int32_t pedalsBrakeGetMaxPosition ( void )
{
    return BreakM.max_position;
}

int32_t pedalsBrakeGetMinPosition ( void )
{
    return BreakM.min_position;
}

/****************************************************************************/
/****************  ACCELERATOR PEDAL MOTOR CONTROL **************************/
/****************************************************************************/


/*
 * @brief    set external DAC output (8bit)
 * @params   accelerator_pedal_pos [0...100%]
 */
void pedalsAcceleratorControl ( float accelerator_pedal_pos )
{
    accelerator_pedal_pos = CLIP_VALUE(accelerator_pedal_pos,0.0,100.0);
    uint8_t val = uint8_map (accelerator_pedal_pos, 0.0, 100.0, ACCELERATOR_DAC_MIN_VAL, ACCELERATOR_DAC_MAX_VAL);
    extDacSetValue( ( uint8_t)( val*0.55 ) , val );
}





/****************************************************************************/
/**********************  CALIBRATION PEDALS  ********************************/
/****************************************************************************/
static bool is_clutch_switch_changed = false;
void pedalsCalibrate ( void )
{
    if ( gazelGetClutchSwitch() ){
        pedalsClutchRelease(10000);
    }
    else{
        pedalsClutchPress(10000);
    }
    if ( gazelGetBrakeSwitch() ){
        pedalsBrakeRelease(10000);
    }
    else{
        pedalsBrakePress(10000);
    }
}

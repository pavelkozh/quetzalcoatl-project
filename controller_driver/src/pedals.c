#include <pedals.h>
#include <lld_control.h>
#include <lld_ext_dac.h>




/****************************************************************************/
/************COMMON FOR BRAKE AND CLUTCH PEDALS MOTOR CONTROL SETUP *********/
/****************************************************************************/


/* encoder ticks units 4000 ticks = 1 rev ( or 5mm translational movement) */
#define CLUTCH_MAX_POS            90000
#define BRAKE_MAX_POS             40000


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
    .max_position    =   CLUTCH_MAX_POS,
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
    .max_position    =   BRAKE_MAX_POS,
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


/*
 * @brief      Perephery setup and lld initialization
 */
void pedalsInit ( void ){
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
int32_t moveClutchPedal (double position, uint16_t speed ){
    MotorRunTracking( &ClutchM,   speed);
    ClutchM.tracked_position = position;
    return  ClutchM.position;
}

/*
 * @brief     Fully press clutch pedal
 * @params    speed - movement speed (look "moveClutchPedal" function description )
 */
void pressClutchPedal ( uint16_t speed ){
    MotorRunContinuous(&ClutchM, 0, speed);
}

/*
 * @brief     Fully release clutch pedal
 * @params    speed - movement speed (look "moveClutchPedal" function description )
 */
void releaseClutchPedal ( uint16_t speed ){
    MotorRunContinuous(&ClutchM, 1, speed);
}

/* Service function */
void calibrateClutchPedal( bool dir, uint16_t speed, uint16_t step ){
    MotorRunCaclibration( &ClutchM, dir, speed, step );
}

/*
 * @brief     stops clutch pedal motor
 */
void stopClutchPedal ( void ){
    MotorStop( &ClutchM );
}

/*
 * @brief     Function can change motor speed  "on the fly"
 * @params    new_speed (look "moveClutchPedal" function description for units )
 */
void changeSpeedClutchPedal ( uint16_t new_speed ){
    MotorSetSpeed( &ClutchM, new_speed );
}

/*******************************/
/* @brief    Get additional information about clutch pedal motor
 * @note     This is not a feedback information,
 *           but a values which microcontroller was send to driver
 */
int32_t getClutchPedalPosition ( void ){
    return ClutchM.position;
}

bool getClutchPedalState ( void ){
    return ClutchM.state;
}

uint8_t getClutchPedalMode ( void ){
    return ClutchM.mode;
}

uint16_t getClutchPedalSpeed ( void ){
    return  ClutchM.pwmd->period;
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
int32_t moveBrakePedal (double position, uint16_t speed ){
    MotorRunTracking( &BreakM,   speed);
    BreakM.tracked_position = position;
    return BreakM.position;
}

/*
 * @brief     Fully press Brake pedal
 * @params    speed - movement speed (look "moveBrakePedal" function description )
 */
void pressBrakePedal ( uint16_t speed ){
    MotorRunContinuous(&BreakM, 0, speed);
}

/*
 * @brief     Fully release Brake pedal
 * @params    speed - movement speed (look "moveBrakePedal" function description )
 */
void releaseBrakePedal ( uint16_t speed ){
    MotorRunContinuous(&BreakM, 1, speed);
}

/* Service function */
void calibrateBrakePedal( bool dir, uint16_t speed, uint16_t step ){
    MotorRunCaclibration( &BreakM, dir, speed, step );
}

/*
 * @brief     stops Brake pedal motor
 */
void stopBrakePedal ( void ){
    MotorStop( &BreakM );
}

/*
 * @brief     Function can change motor speed  "on the fly"
 * @params    new_speed (look "moveClutchPedal" function description for units )
 */
void changeSpeedBrakePedal ( uint16_t new_speed ){
    MotorSetSpeed( &BreakM, new_speed );
}


/* @brief    Get additional information about clutch pedal motor
 * @note     This is not a feedback information,
 *           but a values which microcontroller was send to driver
 */
int32_t getBrakePedalPosition ( void ){
    return BreakM.position;
}

bool getBrakePedalState ( void ){
    return BreakM.state;
}

uint8_t getBrakePedalMode ( void ){
    return BreakM.mode;
}

uint16_t getBrakePedalSpeed ( void ){
    return  BreakM.pwmd->period;
}



/****************************************************************************/
/****************  ACCELERATOR PEDAL MOTOR CONTROL **************************/
/****************************************************************************/


/*
 * @brief    set external DAC output (8bit)
 * @params   accelerator_pedal_pos [0...100%]
 */
void acceleratorPedalControl ( uint8_t accelerator_pedal_pos )
{
    uint8_t val = uint8_map (accelerator_pedal_pos, 0, 100, 57, 240);
    extDacSetValue( ( uint8_t)( val*0.55 ) , val );
}


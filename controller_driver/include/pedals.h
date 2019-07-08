#ifndef INCLUDE_PEDALS_H
#define INCLUDE_PEDALS_H


#include <common.h>



/************COMMON FOR BRAKE AND CLUTCH PEDALS MOTOR CONTROL SETUP *********/

/*
 * @brief      Perephery setup and lld initialization
 */
void pedalsInit ( void );



/*********************** CLUTCH PEDAL MOTOR CONTROL *************************/
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
int32_t pedalsClutchMove (double position, uint16_t speed );

/*
 * @brief     Fully press clutch pedal
 * @params    speed - movement speed (look "moveClutchPedal" function description )
 */
void pedalsClutchPress ( uint16_t speed );

/*
 * @brief     Fully release clutch pedal
 * @params    speed - movement speed (look "moveClutchPedal" function description )
 */
void pedalsClutchRelease ( uint16_t speed );

/* Service function */
void pedalsClutchCalibrate( bool dir, uint16_t speed, uint16_t step );

/*
 * @brief     stops clutch pedal motor
 */
void pedalsClutchStop ( void );

/*
 * @brief     Function can change motor speed  "on the fly"
 * @params    new_speed (look "moveClutchPedal" function description for units )
 */
void pedalsClutchChangeSpeed ( uint16_t new_speed );




/* @brief       Get additional information about clutch pedal motor
 * @note        This is not a feedback information,
 *              but a values which microcontroller was send to driver
 * @return      position [0...BRAKE_MAX_POS] (BRAKE_MAX_POS - define in pedals.h)
 *                  -> 0.0 corresponds to fully released pedal
 *                  -> BRAKE_MAX_POS corresponds to fully pressed pedal
 */
int32_t pedalsClutchGetPosition ( void );

/*
 * @brief        Get additional information about clutch pedal motor
 * @note         This is not a feedback information,
 *               but a values which microcontroller was send to driver
 * @return       1 - moving  //  0 - stop
 */
bool pedalsClutchGetState ( void );

/*
 * @brief        Get additional information about clutch pedal motor
 * @note         This is not a feedback information,
 *               but a values which microcontroller was send to driver
 * @return       2 - MOTOR_MODE_SHOT
 *               3 - MOTOR_MODE_TRACKING
 *               4 - MOTOR_MODE_CONTINUOUS
 *               5 - MOTOR_MODE_CALIBRATION
 */
uint8_t pedalsClutchGetMode ( void );

/* @brief        Get additional information about clutch pedal motor
 * @note         This is not a feedback information,
 *               but a values which microcontroller was send to driver
 * @return       speed [X...500]    -> movement speed in PWM period. *
 *                 -> 500 is a max speed
 *                 -> min speed have not boundary
 */
uint16_t pedalsClutchGetSpeed ( void );


/*********************** BRAKE PEDAL MOTOR CONTROL **************************/


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
int32_t pedalsBrakeMove (double position, uint16_t speed );

/*
 * @brief     Fully press Brake pedal
 * @params    speed - movement speed (look "moveBrakePedal" function description )
 */
void pedalsBrakePress ( uint16_t speed );


/*
 * @brief     Fully release Brake pedal
 * @params    speed - movement speed (look "moveBrakePedal" function description )
 */
void pedalsBrakeRelease ( uint16_t speed );


/* Service function */
void pedalsBrakeCalibrate ( bool dir, uint16_t speed, uint16_t step );


/*
 * @brief     stops Brake pedal motor
 */
void pedalsBrakeStop ( void );


/*
 * @brief     Function can change motor speed  "on the fly"
 * @params    new_speed (look "moveClutchPedal" function description for units )
 */
void pedalsBrakeChangeSpeed ( uint16_t new_speed );




/* @brief       Get additional information about brake pedal motor
 * @note        This is not a feedback information,
 *              but a values which microcontroller was send to driver
 * @return      position [0...BRAKE_MAX_POS] (BRAKE_MAX_POS - define in pedals.h)
 *                  -> 0.0 corresponds to fully released pedal
 *                  -> BRAKE_MAX_POS corresponds to fully pressed pedal
 */
int32_t pedalsBrakeGetPosition ( void );

/*
 * @brief        Get additional information about brake pedal motor
 * @note         This is not a feedback information,
 *               but a values which microcontroller was send to driver
 * @return       1 - moving  //  0 - stop
 */
bool pedalsBrakeGetState ( void );

/*
 * @brief        Get additional information about brake pedal motor
 * @note         This is not a feedback information,
 *               but a values which microcontroller was send to driver
 * @return       2 - MOTOR_MODE_SHOT
 *               3 - MOTOR_MODE_TRACKING
 *               4 - MOTOR_MODE_CONTINUOUS
 *               5 - MOTOR_MODE_CALIBRATION
 */
uint8_t pedalsBrakeGetMode ( void );

/* @brief        Get additional information about brake pedal motor
 * @note         This is not a feedback information,
 *               but a values which microcontroller was send to driver
 * @return       speed [X...500]    -> movement speed in PWM period. *
 *                 -> 500 is a max speed
 *                 -> min speed have not boundary
 */
uint16_t pedalsBrakeGetSpeed ( void );



/****************  ACCELERATOR PEDAL MOTOR CONTROL **************************/

/*
 * @brief    set external DAC output (8bit)
 * @params   accelerator_pedal_pos [0...100%]
 */
void pedalsAcceleratorControl ( float accelerator_pedal_pos );




#endif /* INCLUDE_PEDALS_H */


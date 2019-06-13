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
int32_t moveClutchPedal (double position, uint16_t speed );

/*
 * @brief     Fully press clutch pedal
 * @params    speed - movement speed (look "moveClutchPedal" function description )
 */
void pressClutchPedal ( uint16_t speed );

/*
 * @brief     Fully release clutch pedal
 * @params    speed - movement speed (look "moveClutchPedal" function description )
 */
void releaseClutchPedal ( uint16_t speed );

/* Service function */
void calibrateClutchPedal( bool dir, uint16_t speed, uint16_t step );

/*
 * @brief     stops clutch pedal motor
 */
void stopClutchPedal ( void );

/*
 * @brief     Function can change motor speed  "on the fly"
 * @params    new_speed (look "moveClutchPedal" function description for units )
 */
void changeSpeedClutchPedal ( uint16_t new_speed );


/* @brief    Get additional information about clutch pedal motor
 * @note     This is not a feedback information,
 *           but a values which microcontroller was send to driver
 */
int32_t getClutchPedalPosition ( void );
bool getClutchPedalState ( void );
uint8_t getClutchPedalMode ( void );
uint16_t getClutchPedalSpeed ( void );


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
int32_t moveBrakePedal (double position, uint16_t speed );

/*
 * @brief     Fully press Brake pedal
 * @params    speed - movement speed (look "moveBrakePedal" function description )
 */
void pressBrakePedal ( uint16_t speed );


/*
 * @brief     Fully release Brake pedal
 * @params    speed - movement speed (look "moveBrakePedal" function description )
 */
void releaseBrakePedal ( uint16_t speed );


/* Service function */
void calibrateBrakePedal( bool dir, uint16_t speed, uint16_t step );


/*
 * @brief     stops Brake pedal motor
 */
void stopBrakePedal ( void );


/*
 * @brief     Function can change motor speed  "on the fly"
 * @params    new_speed (look "moveClutchPedal" function description for units )
 */
void changeSpeedBrakePedal ( uint16_t new_speed );

/* @brief    Get additional information about clutch pedal motor
 * @note     This is not a feedback information,
 *           but a values which microcontroller was send to driver
 */
int32_t getBrakePedalPosition ( void );
bool getBrakePedalState ( void );
uint8_t getBrakePedalMode ( void );
uint16_t getBrakePedalSpeed ( void );



/****************  ACCELERATOR PEDAL MOTOR CONTROL **************************/

/*
 * @brief    set external DAC output (8bit)
 * @params   accelerator_pedal_pos [0...100%]
 */
void acceleratorPedalControl ( uint8_t accelerator_pedal_pos );




#endif /* INCLUDE_PEDALS_H */

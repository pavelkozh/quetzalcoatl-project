#ifndef INCLUDE_LLD_STEER_H
#define INCLUDE_LLD_STEER_H

#include <common.h>

/**
 * @brief   Initialize absolute encoder module
 */
void steerEncInit( void );

/**
 * @brief   Read data from absolute encoder 
 * @return  Position from 0 to 360 degrees
 */
uint16_t steerGetPosition( void );//float

/**
 * @brief   Initialize steer motor driver. US talk with driver via SPI
 * @note    2 threads starts and fall to sleep immediately.
 *          (one for start/stop button simulation
 *          + one for direction button simulation)
 */
void steerMotorInit( void );

/*
 * @brief   Send new speed value to driver
 * @param   speed in the range [0...100%]
 * @note    Steer motor driver needs analog input for speed control,
 *          so external DAC with SPI is used
 */
void steerMotorSetSpeed( float speed );

/*
 * @brief   start/stop button (without fixation) simulation
 * @note    for start and then stop driver function needed to be call consistently.
 *          I.e. first call starts driver, second stops, third starts again...
 */
void steerMotorStartStopControl ( void );

/*
 * @brief   Direction change button (without fixation) simulation
 * @note    for change driver's direction of rotation  this function needed to be call consistently.
 *          I.e. first call - dir right, second - dir left, third - dir right...
 */
void steerMotorDirChange ( void );

/*
 * @brief    Check motor state (start or stop)
 * @note     Motor state depends on previous changes on driver's "Start" input
 * @return   1 - if motor is enable (start)
 *           0 - if motor is disable (stop)
 */
bool steerIsMotorEnable ( void );
/*
 * @brief    Check motor direction 	
 * @note     Motor state depends on previous changes on driver's "Start" input
 * @return   1 - if motor is rotates clockwise
 *           0 - if motor is rotates counterсlockwise
 */
bool steerMotorGetDirection( void );
/*
*@brief Inverte flag of steer motor speed
*/

bool steerMotorEnableInvert ( void );
/*
*@brief Inverte flag of steer motor direction
*/

bool steerMotorDirectionInvert( void );


int steerSyncTestDriver();

#endif  /*INCLUDE_LLD_STEER_H */

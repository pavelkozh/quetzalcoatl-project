#ifndef INCLUDE_LLD_ENCODER
#define INCLUDE_LLD_ENCODER

#include <common.h>

/*
 * Hardware description
 * ------------------------------------
 *
 * EXT driver uses input PF13
 * GPT3 used for time intervals between wheel position sensor fronts measurement
 */

/*** Variables ***/
typedef float                   Velocity_t;
typedef float                   Position_t;


/*** Prototypes ***/

/**
 * @ brief                            Gets wheel current velocity value
 *                                    [revolutions per minute (rpm)]
 * @ param[in] ImpsPerRevQuantity     Number of impulses per revolution
 *                                    depends on given sensor
 * @ return                           Current wheel velocity value [rpm]
 */
Velocity_t encoderGetVelocity ( void );





/**
 * @ brief                           Gets wheel current position value
 *                                   [revolutions]
 * @ param[in] ImpsPerRevQuantity    Number of impulses per revolution
 *                                   depends on given sensor
 * @ return                          Current wheel position value [revolutions]
 *
 */
Position_t encoderGetPosition ( void );


#endif /*INCLUDE_LLD_ENCODER*/

#ifndef INCLUDE_LLD_ENCODER_H_
#define INCLUDE_LLD_ENCODER_H_

typedef int32_t    rawEncoderValue_t;
typedef float      rawRevEncoderValue_t;


/**
 * @brief   Initialize periphery connected to encoder
 * @note    Stable for repeated calls
 */
void lldEncoderInit( void );

/**
 * @brief   Get number of encoder ticks
 * @note    Max number of ticks is defined by MAX_TICK_NUM
 * @return  Encoder ticks number depends on direction of rotation
 *          [0; ENC_MAX_TICK_NUM]
 *          after ENC_MAX_TICK_NUM it resets
 */
rawEncoderValue_t lldGetEncoderRawTicks( void );

/**
 * @brief   Get direction of encoder rotation
 * @return  clockwise           -> 0
 *          counterclockwise    -> 1
 */
bool lldGetEncoderDirection( void );

/**
 * @brief   Get number of encoder revolutions [double]
 * @note    1 revolution = MAX_TICK_NUM ticks
 * @return  Encoder revolutions number depends on direction of rotation
 */
rawRevEncoderValue_t   lldGetEncoderRawRevs( void );


/***    In case of ABSOLUTE ENCODER     ***/
/**
 * @brief   Get number of encoder revolutions [int]
 * @note    If you use absolute encoder!!!
 * @return  Encoder revolutions number depends on direction of rotation
 */
rawEncoderValue_t   lldGetAbsoluteEncoderRawRevs( void );

#endif /* INCLUDE_LLD_ENCODER_H_ */


#if 0

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

#endif

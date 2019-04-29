#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#include <ch.h>
#include <hal.h>

#include <stdint.h>
#include <stdlib.h>

#define     PROGRAM_ROUTINE_MASTER                      1
#define     PROGRAM_ROUTINE_TEST_ADC                    2
#define     PROGRAM_ROUTINE_TEST_PWM                    3
#define     PROGRAM_ROUTINE_TEST_TRANSFER_FUNC          4
#define     PROGRAM_ROUTINE_TEST_ENCODER                5
#define     PROGRAM_ROUTINE_TEST_CAN                 	7
#define		PROGRAM_ROUTINE_TEST_ENGINE_SPEED			8
#define     PROGRAM_ROUTINE_TEST_MT_CONTROL             9


#define     MAIN_PROGRAM_ROUTINE                     PROGRAM_ROUTINE_TEST_MT_CONTROL




/**
 * @brief   Initialize EXT driver with empty config
 * @note    Safe to call any times, it checks state of previous call
 * @note    Must be called before EXT driver work
 */
void commonExtDriverInit ( void );

#endif /* INCLUDE_COMMON_H_ */
/**************/
/*** Macros ***/
/**************/

#define CLIP_VALUE(x, min, max) ((x) < (min) ? (min) : \
(x) > (max) ? (max) : (x))

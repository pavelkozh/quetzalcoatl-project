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
//KPP 9 
#define		PROGRAM_ROUTINE_TEST_FUZZY_LOGIC			10
#define     PROGRAM_ROUTINE_TEST_PX4FLOW                11


#define     MAIN_PROGRAM_ROUTINE                     PROGRAM_ROUTINE_TEST_FUZZY_LOGIC




/**
 * @brief   Initialize EXT driver with empty config
 * @note    Safe to call any times, it checks state of previous call
 * @note    Must be called before EXT driver work
 */
void commonExtDriverInit ( void );


/**************/
/*** Macros ***/
/**************/

#define CLIP_VALUE(x, min, max) ((x) < (min) ? (min) : \
(x) > (max) ? (max) : (x))

#endif /* INCLUDE_COMMON_H_ */
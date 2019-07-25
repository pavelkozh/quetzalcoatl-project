#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#include <ch.h>
#include <hal.h>

#include <MT.h>
#include <MT_control.h>
#include <feedback.h> 
#include <pedals.h>
#include <speed.h>
#include <lld_steer.h>
#include <steer_control.h>
#include <emergency_stop.h>
#include <engine_ignition.h>
#include <lld_steer_step_motor.h>
#include <sound_signal.h>

#include <communication.h>


#include <stdint.h>
#include <stdlib.h>
#include <errno.h>


#define     PROGRAM_ROUTINE_MASTER                          1

#define		PROGRAM_ROUTINE_TEST_FUZZY_LOGIC			    2
#define     PROGRAM_ROUTINE_TEST_PX4FLOW                    3
#define     PROGRAM_ROUTINE_TEST_PEDALS                     4
#define     PROGRAM_ROUTINE_TEST_MT_CONTROL                 5
#define     PROGRAM_ROUTINE_TEST_SPEED                      6
#define     PROGRAM_ROUTINE_TEST_HSC                        7
#define 	PROGRAM_ROUTINE_TEST_EMERGENCY_STOP			    8
#define     PROGRAM_ROUTINE_TEST_STEER                      9
#define     PROGRAM_ROUTINE_TEST_COMMUNICATION              10
#define     PROGRAM_ROUTINE_TEST_ENGINE_IGNITION            11
#define     PROGRAM_ROUTINE_TEST_STEER_POS_LOOP             12
#define     PROGRAM_ROUTINE_TEST_STEER_WITH_COMMUNICATION   13
#define		PROGRAM_ROUTINE_TEST_MAIN			            14
#define     PROGRAM_ROUTINE_TEST_STEER_SM                   15
#define     PROGRAM_ROUTINE_TEST_SOUND_SIGNAL               16


#define     MAIN_PROGRAM_ROUTINE  PROGRAM_ROUTINE_TEST_MAIN

/**************/
/*** Macros ***/
/**************/

#define CLIP_VALUE(x, min, max) ((x) < (min) ? (min) : \
(x) > (max) ? (max) : (x))

#ifndef EOK
    #define EOK 0
#endif

/**
 * @brief   Initialize EXT driver with empty config
 * @note    Safe to call any times, it checks state of previous call
 * @note    Must be called before EXT driver work
 */
void commonExtDriverInit ( void );


/**
 * @brief
 */
uint8_t uint8_map(uint8_t x, uint8_t in_min, uint8_t in_max, uint8_t out_min, uint8_t out_max);
uint32_t uint32_map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max);
double double_map(double x, double in_min, double in_max, double out_min, double out_max);
int8_t sign ( float var );


#endif /* INCLUDE_COMMON_H_ */


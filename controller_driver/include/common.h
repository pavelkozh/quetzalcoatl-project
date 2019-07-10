#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#include <ch.h>
#include <hal.h>
#include <MT.h>
#include <MT_control.h>
#include <feedback.h> 
#include <pedals.h>
#include <speed.h>
#include <steer.h>
#include <emergency_stop.h>

#include <stdint.h>
#include <stdlib.h>

#define     PROGRAM_ROUTINE_MASTER                      1


#define		PROGRAM_ROUTINE_TEST_FUZZY_LOGIC			2
#define     PROGRAM_ROUTINE_TEST_PX4FLOW                3
#define     PROGRAM_ROUTINE_TEST_PEDALS                 4
#define     PROGRAM_ROUTINE_TEST_MT_CONTROL             5
#define     PROGRAM_ROUTINE_TEST_SPEED                  6
#define     PROGRAM_ROUTINE_TEST_HSC                    7
#define 	PROGRAM_ROUTINE_TEST_EMERGENCY_STOP			8
#define     PROGRAM_ROUTINE_TEST_STEER                  9


#define     MAIN_PROGRAM_ROUTINE                     PROGRAM_ROUTINE_MASTER






/**************/
/*** Macros ***/
/**************/

#define CLIP_VALUE(x, min, max) ((x) < (min) ? (min) : \
(x) > (max) ? (max) : (x))


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


#endif /* INCLUDE_COMMON_H_ */


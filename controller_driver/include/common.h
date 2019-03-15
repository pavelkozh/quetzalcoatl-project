#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_


#include <ch.h>
#include <hal.h>

#include <stdint.h>
#include <stdlib.h>

#define     PROGRAM_ROUTINE_MASTER                      0
#define     PROGRAM_ROUTINE_TEST_ADC                    1
#define     PROGRAM_ROUTINE_TEST_PWM                    2
#define     PROGRAM_ROUTINE_TEST_TRANSFER_FUNC          3
#define     PROGRAM_ROUTINE_TEST_ENCODER                4


#define     MAIN_PROGRAM_ROUTINE              PROGRAM_ROUTINE_TEST_ENCODER



/**
 * @brief   Initialize EXT driver with empty config
 * @note    Safe to call any times, it checks state of previous call
 * @note    Must be called before EXT driver work
 */
void commonExtDriverInit ( void );

#endif /* INCLUDE_COMMON_H_ */

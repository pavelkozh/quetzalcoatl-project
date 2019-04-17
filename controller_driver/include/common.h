#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#include <ch.h>
#include <hal.h>

#include <stdint.h>
#include <stdlib.h>

      

#define     PROGRAM_ROUTINE_MASTER                   0
#define     PROGRAM_ROUTINE_TEST_CAN                 1
#define		PROGRAM_ROUTINE_TEST_ENGINE_SPEED		 10


#define     MAIN_PROGRAM_ROUTINE                     PROGRAM_ROUTINE_TEST_ENGINE_SPEED



#endif /* INCLUDE_COMMON_H_ */
/**************/
/*** Macros ***/
/**************/

#define CLIP_VALUE(x, min, max) ((x) < (min) ? (min) : \
(x) > (max) ? (max) : (x))
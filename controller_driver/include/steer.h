#ifndef INCLUDE_STEER_H 
#define INCLUDE_STEER_H 

#include <common.h>

/**
 * @brief   Initialize absolute encoder module
 */
void steerEncInit( void );
void  steerEncReadValue( void );

/**
 * @brief   Read data from absolute encoder 
 * @return  Position from 0 to 360 degrees
 */
double steerGetPosition( void );

#endif  /*INCLUDE_STEER_H */
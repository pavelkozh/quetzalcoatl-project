#ifndef _COMMUNICATION_
#define _COMMUNICATION_

#include <stdint.h>

#define COMM_MODE_SERIAL_PROG   1
#define COMM_MODE_SERIAL_USB    2

#define COMM_MODE   COMM_MODE_SERIAL_USB

typedef int8_t comm_speed_t;
typedef int8_t comm_steer_t;

void comm_init();
void comm_dbgprintf( const char* format, ... );
comm_speed_t comm_get_speed( void );
comm_steer_t comm_get_steer( void );

#endif //_COMMUNICATION_

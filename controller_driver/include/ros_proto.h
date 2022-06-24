#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>

/*** ROS prototypes ***/

typedef struct gazel_ros_send_state
{   
    // km/h
    float linear_speed;
    // rad
    float steering_angle;
}gazel_ros_send_state_t;


/**
 * @brief   Init ROS parameters
 * @param   prio    Priority of thread
 */
void rosInit( tprio_t prio );

void rosSendState( gazel_ros_send_state_t state );

#ifdef __cplusplus
}
#endif

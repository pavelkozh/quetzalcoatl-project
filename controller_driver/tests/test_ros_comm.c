#include <tests.h>
#include <chprintf.h>

#include <common.h>
#include <ros_proto.h>


void testRosComm(void)
{
    /* For USB debug */
    comm_init(NULL, 0, false);

    rosInit(NORMALPRIO);

    gazel_ros_send_state_t state = {
        .linear_speed = 12,
        .steering_angle = 0.1
    };

    while (1)
    {
        comm_dbgprintf_info("Test print");
        rosSendState(state);
        chThdSleepMilliseconds(1000);
    }
}

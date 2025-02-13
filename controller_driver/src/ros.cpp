#include <ros_proto.h>
#include <common.h>
#include <ros.h>
#include <ros/node_handle.h>
#include <chprintf.h>
/**************/
/* ROS things */
/**************/

#include <std_msgs/UInt8.h>
#include <std_msgs/Int8.h>
#include <quetzalcoatl_msgs/GazelState.h>


#define THREAD_SLEEP_TIME 10

/* Useful for watchdog */

// static virtual_timer_t  watchdog_vt;
// static void watchdog_cb(void *arg)
// {
//     (void)arg;
//     steerExtTask = speedExtTask = 0;
// }
// chVTSet( &watchdog_vt, MS2ST( CONTROL_SET_TIMEOUT_MS ), watchdog_cb, NULL );

/* =================== */

ros::NodeHandle ros_node;

extern double  vs;
extern float steer_angle_rad;
extern int8_t mt_shifting;
extern int8_t flag_joystick;
extern float clutch_pos;

void cmd_cb(const quetzalcoatl_msgs::GazelState &msg)
{
//    comm_dbgprintf_info("New command: %d, %d", (int)(msg.linear_speed*10), (int)(msg.angle_steering*10));
    //chprintf( (BaseSequentialStream *)&SD3, "New command: %d, %d\n\r", (int)(msg.linear_speed*10), (int)(msg.angle_steering*10));
    // TODO - call here your functions to write new targets

    /*

    double vst = ((double)(msg.linear_speed))*3.6;// Vref update
    if(vst<=0){
        vs=0;
    }else{
        vs=vst;
    }

    */

    palToggleLine(LINE_LED2);

    float cl_pos = ((float)(msg.linear_speed))*70000/100;// Vref update
    if(cl_pos<=0){
        clutch_pos=0;
    }else{
        clutch_pos=cl_pos;
    }

    steer_angle_rad = -1.0*(float)(msg.angle_steering);

    flag_joystick=1;


}

ros::Subscriber<quetzalcoatl_msgs::GazelState> topic_in_state("state_cmd", &cmd_cb);
quetzalcoatl_msgs::GazelState outMsg;
ros::Publisher topic_out_state("state", &outMsg);
//=======================================================

float st2s = 1.0 / CH_CFG_ST_FREQUENCY;
bool initialized = false;

// static float get_seconds()
// {
//     return chVTGetSystemTimeX() * st2s;
// }

static void setTime(ros::Time &stamp)
{
    RosSocketHardware* hardware = ros_node.getHardware();

    float curr_time_ms = hardware->time();
    float seconds;
    float fract_part = modf(curr_time_ms/1000, &seconds);

    stamp.sec = seconds;
    stamp.nsec = fract_part * 1e9;
}

void rosSendState(gazel_ros_send_state_t state)
{
    if (!initialized) {
        return;
    }

    outMsg.linear_speed = state.linear_speed;
    outMsg.angle_steering = state.steering_angle;

    setTime(outMsg.stamp);

    topic_out_state.publish(&outMsg);

}

/*
 * ROS spin thread - used to receive messages
 */

static THD_WORKING_AREA(waSpinner, 1024);
static THD_FUNCTION(Spinner, arg)
{
    (void)arg;
    chRegSetThreadName("ROS Spinner");

    initialized = true;

    comm_dbgprintf_info("Start spinning");
    int count = 20;
    while (true)
    {
        systime_t time = chVTGetSystemTimeX();

        int spin_status = ros_node.spinOnce();
        if (0 != spin_status ) {
            comm_dbgprintf_info("Spin failed: %d", spin_status);
        }

        chThdSleepUntilWindowed( time, time + MS2ST( 20 ) );

        if (count++ > 20) {
            count = 0;
            chprintf( (BaseSequentialStream *)&SD3, "ROS process\n\r");
        }
    }
}

void rosInit(tprio_t prio)
{

    /* ROS setup */
    ros_node.initNode();
    ros_node.setSpinTimeout(500);

    /* ROS publishers */
    ros_node.advertise(topic_out_state);

    /* ROS subscribers */
    ros_node.subscribe(topic_in_state);
    /* ROS service client */

    /* Main ROS thread */
    chThdCreateStatic(waSpinner, sizeof(waSpinner), prio, Spinner, NULL);
    chprintf( (BaseSequentialStream *)&SD3, "ROS thread init\n\r");

}

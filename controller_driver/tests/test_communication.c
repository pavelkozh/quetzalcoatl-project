#include <ch.h>
#include <hal.h>
#include <communication.h>

/* For module test.
Obtaining speed and angle values and sending them is processed.
*/

// typedef struct {
//     void (*on_set)(uint8_t speed, uint8_t angle);
//     void (*on_start)(void);
//     void (*on_stop)(void);
// } structEventFun_t;

void funct_on_stop(void){
    palToggleLine(LINE_LED1);
    chThdSleepMilliseconds(200);
}

void funct_on_start(void){
    palToggleLine(LINE_LED2);
    chThdSleepMilliseconds(200);
}

void fucnt_on_set(uint8_t speed, uint8_t street)
{
    palToggleLine(LINE_LED3);
    chThdSleepMilliseconds(200);
}

int main(void)
{
    chSysInit();
    halInit();

    // funcEvent_t structForFunc = getDefaultCfg();
    communicationEventFun_t structForFunc = {NULL, NULL, NULL};
 
    structForFunc.on_set = &fucnt_on_set;
    structForFunc.on_start =&funct_on_start;
    structForFunc.on_stop =&funct_on_stop;
    
    comm_init(structForFunc);

    comm_speed_t value_speed_dbg = 0;
    comm_steer_t value_angle_dbg = 0;

    

    while ( true )
    {   
        // value_speed_dbg = comm_get_speed();
        // value_angle_dbg = comm_get_steer();

        // comm_dbgprintf_info("value speed dbg %d, value angle dbg %d\n", value_speed_dbg, value_angle_dbg);

        // comm_dbgprintf_warning("Warning!\n");
        // comm_dbgprintf_error("Oops... Error\n");
        // palToggleLine(LINE_LED3);
        chThdSleepMilliseconds(200);
    }
}

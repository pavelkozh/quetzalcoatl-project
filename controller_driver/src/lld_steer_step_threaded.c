#include <lld_steer_step_threaded.h>

/*********************/
/*** Configuration ***/
/*********************/

#define LINE_STEP   PAL_LINE(GPIOB, 14U)
#define LINE_DIR    PAL_LINE(GPIOB, 15U)

#define STEP_HALF_DELAY_US  10

#define SET_POSITIVE_DIR()  (palSetLine(LINE_DIR))
#define SET_NEGATIVE_DIR()  (palClearLine(LINE_DIR))

#define MB_BUFFER_SZ    20

/*************************/
/*** Configuration end ***/
/*************************/

mailbox_t steps_mb;
msg_t steps_mb_buffer[MB_BUFFER_SZ];

static THD_WORKING_AREA(waSteerStepThd, 256);
static THD_FUNCTION(SteerStepThd, arg)
{
    arg = arg;

    msg_t steps_msg;

    while (true)
    {
        msg_t service_msg = chMBFetchTimeout(&steps_mb, &steps_msg, TIME_INFINITE);
        if (service_msg == MSG_TIMEOUT)
            continue;
        else if (service_msg == MSG_RESET)
            break;      // Buffer is broken

        steer_task_steps_t ref = steps_msg;
        
        if (ref < 0)
        {
            SET_NEGATIVE_DIR();
            ref = -ref;
        }
        else
        {
            SET_POSITIVE_DIR();
        }   

        for (int i = 0; i < ref; i++)
        {
            palSetLine(LINE_STEP);
            chThdSleepMicroseconds(STEP_HALF_DELAY_US);
            palClearLine(LINE_STEP);
            chThdSleepMicroseconds(STEP_HALF_DELAY_US);
        }
    }
}

void steerStepThdInit()
{
    palSetLineMode(LINE_STEP, PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(LINE_DIR, PAL_MODE_OUTPUT_PUSHPULL);

    palClearLine(LINE_STEP);
    palClearLine(LINE_DIR);

    chMBObjectInit(&steps_mb, steps_mb_buffer, MB_BUFFER_SZ);
    chThdCreateStatic(waSteerStepThd, sizeof(waSteerStepThd), NORMALPRIO, SteerStepThd, NULL /* arg is NULL */);
}

int steerStepThdMakeSteps(steer_task_steps_t steps)
{
    msg_t steps_msg = steps;

    msg_t msg = chMBPostTimeout(&steps_mb, steps_msg, TIME_IMMEDIATE);

    if (msg == MSG_OK)
        return EOK;
    else if (msg == MSG_TIMEOUT)
        return EOVERFLOW;
    else
        return EFAULT;
}

bool steerStepThdIsCompleted(void)
{
    chSysLock();
    size_t result = chMBGetUsedCountI();
    chSchRescheduleS();
    chSysUnlock();

    return result == 0;
}

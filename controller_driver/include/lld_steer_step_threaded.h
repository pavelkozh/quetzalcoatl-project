#ifndef LLD_STEER_STEP_THREADED_H
#define LLD_STEER_STEP_THREADED_H

#include <common.h>

typedef int32_t steer_task_steps_t;

 /**
  * @brief:  Initialization function
  */
void steerStepThdInit(void);

/**
 * @brief:  Set step for the task to proceed
 * @param:  steps - Amout of steps to proceed, can be negative to rotate the opposite direction
 * @return: EOK       - Task added
 *          EOVERFLOW - Buffer is overfilled, task is not added
 *          EFAULT    - Buffer is broken, failed to add task
 * @note:   The system uses buffering of tasks, so don`t forget about latency 
 */
int steerStepThdMakeSteps(steer_task_steps_t steps);

/**
 * @brief:  Check if all tasks are completed
 * @note:   Function uses reschedule, don`t use it too often
 */
bool steerStepThdIsCompleted(void);

#endif // LLD_STEER_STEP_THREADED_H
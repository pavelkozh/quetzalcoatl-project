#include <common.h>
#include <tests.h>
#include <chprintf.h>



int main(void)
{
    chSysInit();
    halInit();


#if (MAIN_PROGRAM_ROUTINE != PROGRAM_ROUTINE_MASTER) // need fixing

    testsRoutines();

#else
    while(1){
        chThdSleepMilliseconds(1000);
    };
#endif
}
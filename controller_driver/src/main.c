#include <common.h>
#include <tests.h>
#include <chprintf.h>



int main(void)
{
    chSysInit();
    halInit();

    //commonExtDriverInit();

    #if (MAIN_PROGRAM_ROUTINE != PROGRAM_ROUTINE_MASTER) // need fixing

        testsRoutines();

    #else



    #endif
}

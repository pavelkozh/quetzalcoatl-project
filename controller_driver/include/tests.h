#ifndef INCLUDE_TESTS_H_
#define INCLUDE_TESTS_H_

#include <common.h>



void TestPedals ( void );
void TestMTControl ( void );
void TestSpeed ( void );




static inline void testsRoutines( void )
{

#if (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_PEDALS)

    TestPedals ();
#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_MT_CONTROL)
    TestMTControl ();
#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_SPEED)
    TestSpeed ();

#endif
}


#endif /* INCLUDE_TESTS_H_ */

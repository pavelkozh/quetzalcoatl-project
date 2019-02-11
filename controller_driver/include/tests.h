#ifndef INCLUDE_TESTS_H_
#define INCLUDE_TESTS_H_

#include <common.h>

void TestADCRouting ( void );






static inline void testsRoutines( void )
{
#if (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_ADC)

    TestADCRouting();

#endif
}


#endif /* INCLUDE_TESTS_H_ */

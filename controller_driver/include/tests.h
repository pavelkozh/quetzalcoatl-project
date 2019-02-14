#ifndef INCLUDE_TESTS_H_
#define INCLUDE_TESTS_H_

#include <common.h>

void TestADCRouting ( void );

void TestPWMRouting (void);

void testTFCalcRouting ( void );






static inline void testsRoutines( void )
{
#if (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_ADC)

    TestADCRouting();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_PWM)

    TestPWMRouting();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_TRANSFER_FUNC)

    testTFCalcRouting();

#endif
}


#endif /* INCLUDE_TESTS_H_ */

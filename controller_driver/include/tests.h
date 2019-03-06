#ifndef INCLUDE_TESTS_H_
#define INCLUDE_TESTS_H_

#include <common.h>


/*
 * @brief   Routine of ADC module testing
 * @note    The routine has internal infinite loop
 * @note    SD7 is used for testing (PE7, PE8)
 */
void TestADCRouting ( void );


/*
 * @brief   Routine of PWM module testing
 * @note    The routine has internal infinite loop
 * @note    SD7 is used for testing (PE7, PE8)
 */
void TestPWMRouting (void);


/*
 * @brief   Routine of transfer function calculation module testing
 * @note    The routine has internal infinite loop
 * @note    SD7 is used for testing (PE7, PE8)
 */
void testTFCalcRouting ( void );


/*
 * @brief  Routing for encoder module testing
 */
void TestEncoderRouting (void);






static inline void testsRoutines( void )
{
#if (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_ADC)

    TestADCRouting();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_PWM)

    TestPWMRouting();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_TRANSFER_FUNC)

    testTFCalcRouting();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_ENCODER)

    TestEncoderRouting();

#endif
}


#endif /* INCLUDE_TESTS_H_ */

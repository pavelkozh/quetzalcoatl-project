#ifndef INCLUDE_TESTS_H_
#define INCLUDE_TESTS_H_

#include <common.h>

/*
 * @brief   Routine of CAN controller testing
 * @note    The routine has internal infinite loop
 * @note    SD7 is used for testing (PE7, PE8)
 */

/*  Hardware connection for simulation
 *  CANRX (PD_0), CANTX (PD_1)
 *  Direct control of PA0 - led
 */
void TestEngineSpeedRouting ( void );

/* @brief test fuzzy_logic
* 
*/
void TestFLRouting(void);

/* 
* @brief test px3flow sensore
*/
void PX4FLOWTest(void);



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
#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_FUZZY_LOGIC)

    TestFLRouting();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_ENGINE_SPEED)

    TestEngineSpeedRouting();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_PX4FLOW)

    PX4FLOWTest();

#endif

}


#endif /* INCLUDE_TESTS_H_ */
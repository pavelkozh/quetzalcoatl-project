#ifndef INCLUDE_TESTS_H_
#define INCLUDE_TESTS_H_

#include <common.h>


/*
* @brief test fuzzy_logic
*/
void TestFLRouting(void);

/* 
* @brief test px3flow sensor
*/
void PX4FLOWTest(void);
void TestPedals ( void );
void TestMTControl ( void );
void TestSpeed ( void );
void TestHSC1 ( void );
void testSteer ( void );
void testSteerPositionControl ( void );
void testEmergencyStop ( void );
void TestEngIgnition ( void );
void testCommunication(void);


static inline void testsRoutines( void )
{
	
#if (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_PEDALS)
    TestPedals ();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_MT_CONTROL)
    TestMTControl ();
    
#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_SPEED)
    TestSpeed ();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_HSC)
    TestHSC1();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_STEER)
    testSteer();
    
#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_STEER_POS_LOOP)
    testSteerPositionControl();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_EMERGENCY_STOP)
    testEmergencyStop();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_FUZZY_LOGIC)
    TestFLRouting();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_ENGINE_IGNITION)
    TestEngIgnition();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_COMMUNICATION)
    testCommunication();


#endif

}


#endif /* INCLUDE_TESTS_H_ */

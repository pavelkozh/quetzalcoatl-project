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
void testMain( void );
void TestSteerSM ( void );
void TestSoundSignal( void );
void testMainNew( void );
void testRosComm(void);
void testAnalogEncoder(void);

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

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_STEER_SM)
    TestSteerSM();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_EMERGENCY_STOP)
    testEmergencyStop();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_FUZZY_LOGIC)
    TestFLRouting();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_ENGINE_IGNITION)
    TestEngIgnition();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_COMMUNICATION)
    testCommunication();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_STEER_WITH_COMMUNICATION)
    testCommunicationWithSteer();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_SOUND_SIGNAL)
    TestSoundSignal();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_START)
    TestStart();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_MAIN)
    testMain();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_MAIN_NEW)
    testMainNew();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_ROS_COMM)
    testRosComm();

#elif (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_ANALOG_ENCODER)
    testAnalogEncoder();

#endif

}


#endif /* INCLUDE_TESTS_H_ */

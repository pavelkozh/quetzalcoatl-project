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
void TestCanRouting ( void );






static inline void testsRoutines( void )
{
#if (MAIN_PROGRAM_ROUTINE == PROGRAM_ROUTINE_TEST_CAN)

    TestCanRouting();

#endif
}


#endif /* INCLUDE_TESTS_H_ */

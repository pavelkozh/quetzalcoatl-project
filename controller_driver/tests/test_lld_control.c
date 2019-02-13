#include <tests.h>
#include <lld_control.h>


void TestPWMRouting (void)
{
  PWMUnitInit();

  while (1)
  {
    chThdSleepMilliseconds( 500 );
  }
}

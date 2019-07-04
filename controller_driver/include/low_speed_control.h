#ifndef LOW_SPEED_CONTROL
#define LOW_SPEED_CONTROL
#include <fuzzy_logic.h>
#include <feedback.h> 
void lowSpeedStart(void);
void lowSpeedStop(void);
void lowSpeedSetReference( double ref);
void lowSpeedControlIntit(void);
double lowSpeedGetErr(void);
double lowSpeedGetdErr(void);

#endif //LOW_SPEED_CONTROL

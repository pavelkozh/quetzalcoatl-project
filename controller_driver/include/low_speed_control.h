#ifndef LOW_SPEED_CONTROL
#define LOW_SPEED_CONTROL
#include <fuzzy_logic.h>
#include <feedback.h> 
void lowSpeedStart(void);
void lowSpeedStop(void);
void lowSpeedSetReference( double ref);
void lowSpeedControlIntit(void);
void lowSpeedGetErr(void);
void lowSpeedGetdErr(void);

#endif //LOW_SPEED_CONTROL
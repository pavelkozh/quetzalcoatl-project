#ifndef ENGINE_IGNITION_H
#define ENGINE_IGNITION_H

#include <common.h>

void engIgnitionInit ( void );
bool engStarterSwitchOn ( void );
void engStarterSwitchOff ( void );

void engIgnitionSwitchOn ( void );
void engIgnitionSwitchOff ( void );


/* Debug functions */
void engIgnitionDbgSetEngSpeed ( uint16_t val );
uint8_t engIgnitionDbgGetThreadEnFlag ( void );
uint8_t engIgnitionDbgGetFlaf ( void );




#endif /* ENGINE_IGNITION_H */

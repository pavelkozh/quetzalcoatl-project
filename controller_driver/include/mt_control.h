#ifndef INCLUDE_MT_CONTROL
#define INCLUDE_MT_CONTROL

#include <common.h>

/*
 * @brief     perephery initialization
 *            driver control unit initialization
 */
void mtControlInit ( void );


/*
 * @brief    move carriage to neutral position
 */
void shiftMTToNeutral ( void );
void shiftToFirstGear ( void );
void shiftToSecondGear ( void );
void shiftToThirdGear ( void );
void shiftToForthGear ( void );
void shiftToFifthGear ( void );
void shiftToReverseGear ( void );

void verticalCaclibration( bool dir, uint16_t speed, uint16_t step );
void gorisontalCaclibration( bool dir, uint16_t speed, uint16_t step );

int32_t getVerticalPosition ( void );
int32_t getGorisontalPosition ( void );
int32_t getVerticalMaxPosition ( void );
int32_t getGorisontalMaxPosition ( void );
int32_t getVerticalTargetPosition ( void );
int32_t getGorisontalTargetPosition ( void );
bool getGorisontalState ( void );
bool getVerticalState ( void );
uint8_t getGorisontalMode ( void );
uint8_t getVerticalMode ( void );
uint16_t getGorisontalSpeed ( void );
uint16_t getVerticalSpeed ( void );



#endif /* INCLUDE_MT_CONTROL */

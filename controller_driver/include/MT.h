#ifndef INCLUDE_MT_CONTROL
#define INCLUDE_MT_CONTROL

#include <common.h>




/*
 * @brief     periphery initialization
 *            driver control unit initialization
 * @note      In function sets up (x,y) coordinates of "gear" positions
 */
void mtMotorsControlInit ( void );

/*
 * @brief    move carriage to neutral position
 */
int8_t shiftMTToNeutral ( uint16_t speed );

/*
 * @brief    move carriage to position corresponding to selected gear
 * @note     attention!!! function need to be permanently called to execute properly
 */
int8_t shiftMTToNextGear (int8_t gear_num, uint16_t speed);

/*
 * @brief    External interrupts initialization for calibration
 * @note     For calibration used 4 limit switches
 */
void calibrationMTInit ( void );

/*
 * @brief    moving carriage to the center
 * @note     NOT WORKING!!!!!!!!!!!!!!!!
 */
uint8_t doCalibrationMT ( void );





///////////////////////////////////////////////////////////////////

/*
 * @brief    Service functions
 */
void verticalCaclibration( bool dir, uint16_t speed, uint16_t step );
void gorisontalCaclibration( bool dir, uint16_t speed, uint16_t step );
void setTrackedMode ( uint16_t vertical_speed, uint16_t gorisontal_speed );
void setTrackedModePositionVerticalMotor ( int32_t pos );
void setTrackedModePositionGorisontalMotor ( int32_t pos );
void verticalMotorRunContinuous (bool direction, uint16_t speed);
void gorisontalMotorRunContinuous (bool direction, uint16_t speed);
void verticalMotorStop ( void );
void gorisontalMotorStop ( void );

void setVerticalMotorMaxPosition ( int32_t max_pos );
void setGorisontalMotorMaxPosition ( int32_t max_pos );

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

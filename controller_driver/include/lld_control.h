#ifndef INCLUDE_LLD_CONTROL_H
#define INCLUDE_LLD_CONTROL_H

#include <common.h>

/*
 * Hardware description
 * ------------------------------------
 *
 * EXT driver uses inputs PE4, PE5
 * PWM (pin PE9) uses TIM1
 * PE11, PF14 uses as general outputs.
 */



/******************/
/***** Define *****/
/******************/
#define MOTOR_STOPED            0
#define MOTOR_MOVING            1
#define MOTOR_MODE_SHOT         2
#define MOTOR_MODE_TRACKING     3
#define MOTOR_MODE_CONTINUOUS   4
#define MOTOR_MODE_CALIBRATION  5




/******************/
/*** Structure ***/
/******************/

typedef struct {
 
 /**
  * @brief:  pointer to a @p PWMDriver object for motor control.
  */
    PWMDriver           *pwmd;

 /**
  * @brief:  Rising edge callback. 
  * @note:  This callback is invoked on PWM counter reset. If set to
  *        @p NULL then the callback is disabled.
  */
    pwmcallback_t       rising_edge_cb;
 
 /**
  * @brief:  Falling edge callback. 
  * @note:  This callback is invoked on the channel compare event after service callback. If set to
  *        @p NULL then the callback is disabled.
  */
    pwmcallback_t       falling_edge_cb;
 /**
  * @brief: Absolute position in ticks of encoder relative to the zero point. 
  * @note:  Read only, but you can use resetPosition() to set 0 value. Motor can
  *         move 0 to max_position
  */
    int32_t             position;
 /**
  * @brief: Max motor position. 
  */
    int32_t             max_position;

 /**
  * @brief: Tracking motor position. 
  * @note:  Use with MOTOR_RUN_TRACKING state 
  */
    int32_t             tracked_position;


 /**
  * @brief: speed of motor ( Period of PWM timer )
  */
    //uint16_t          speed;

 /**
  * @brief: direction of rotation.
  */
    //bool              dir;
 
 /**
  * @brief: line for control direction of rotation.
  */
    ioline_t                dir_line;


  /**
  * @brief: state of motor.
  */
    bool                state;

  /**
  * @brief: Mode of motor control.
  */
    uint8_t               mode;
} MotorDriver;



/******************/
/*** Prototypes ***/
/******************/

/*
* @brief:  Rising callcallback for all motor driver.
* @note:  It need to include in user callback
* @param:  mtd - Motor Driver
*/
void risingEdgeCb(MotorDriver *mtd);
/*
* @brief:  Falling callback for all motor driver.
* @note:   It need to include in user callback
* @param:  mtd - Motor Driver
*/
void fallingEdgeCb(MotorDriver *mtd);


/*
 * @brief: lld control module initialization
 *         EXT driver initialization (buttons enable/disable, start/stop,
 *                                    left/right direction)
 *         setup PUL, DIR, EN control pins
 */

void MotorlldControlInit ( MotorDriver *mtd );


/*
 * @brief:   enable PWM channel if enable button pressed
 * @note :   duty cycle always 50%
 * @param:   mtd - Motor Driver
 *           dir - direction
 *           ang - angle [pulse] (4000 pulse per revolution)
 */
void MotorRunContinuous( MotorDriver *mtd, bool dir, uint16_t speed);


/*
 * @brief :  drive run in tracking mode
 * @params:  mtd - Motor Driver
 *           speed - speed of rotutuion in ticks of pwm timer
 */
void MotorRunTracking(MotorDriver *mtd, uint16_t speed);

/*
 * @brief :  drive run in tracking mode
 * @params:  mtd - Motor Driver
 *           dir - direction rotation
 *           speed - speed of rotutuion in ticks of pwm timer
 *           pulses - impulses of the position to be shifted
 */
void MotorRunCaclibration( MotorDriver *mtd, bool dir, uint16_t speed, uint32_t pulses);

/*
 * @brief :  set motor's rotation direction
 * @params:  mtd - Motor Driver
 *           dir - direction rotation
 */
void MotorSetDirection( MotorDriver *mtd, bool dir );

/**
 * @brief : function resets pwm pulses counter
 */
void MotoresetPwmCounter ( void );

/*
 * @brief :  set motor's rotation speed
 * @params:  speed - speed of rotation (max speed - 100kHz = 1500rpm)
 */
void MotorSetSpeed( MotorDriver *mtd, uint16_t speed );

/*
 * @brief :  drive stop
 * @params:  mtd - Motor Driver
 */
void MotorStop( MotorDriver *mtd );


/*
 * @brief :  Reset position and tracked_position
 * @params:  mtd - Motor Driver
 */
void MotorResetPotision ( MotorDriver *mtd );

#endif /* INCLUDE_LLD_CONTROL_H */

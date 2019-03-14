#include <lld_odometry.h>
#include <lld_encoder.h>

bool                is_lld_odometry_Initialized = false;

/*
 * @brief :  Initialize periphery connected to odometry unit
 * @note  :  Stable for repeated calls
 */
void lldOdometryInit( void )
{
    if ( is_lld_odometry_Initialized )
            return;
#if 0
    /*** Start working GPT driver in asynchronous mode ***/
    gptStart( gptDriver, &gpt2cfg );
    uint32_t gpt_period = gptFreq * 0.01;   // 10 ms
    gptStartContinuous( gptDriver, gpt_period );
#endif
    chVTObjectInit(&odom_update_vt);
    chVTSet( &odom_update_vt, MS2ST( VT_ODOM_MS ), odom_update_vt_cb, NULL );


    lldEncoderInit( );



    /***    Set initialization flag     ***/
    is_lld_odometry_Initialized = true;
}




/*The distance that passes the carriage per 1 revolution of the shaft [mm]*/
#define LINEAR_MOVEMENT_STEP_MM      5
#define SHAFT_LENGTH_MM              300 /*mm*/

/*
 * @brief :  Function converts the number of revolutions of the motor to the
 *           carriage linear position in millimeters
 * @params:  Number of motor revolutions after initialization. Clockwise
 *           rotation increments revolution quantity.Counterclockwise - decrements.
 * @return:  Carriage linear position in the range (0...SHAFT_LENGTH_MM) [mm]
 */
float rev2linearPos ( float rev )
{
    return rev * (float)LINEAR_MOVEMENT_STEP_MM;
}


/*
 * @brief :  Function check if the linear position carriage is close
 *            to the end of the shaft.
 * @params:  lin_pos [mm] in the range (0...SHAFT_LENGTH_MM)
 * @return:  1 - if alarm // 0 - if ok
 */
bool ifCarriageCloseToEnd (uint16_t lin_pos) //If it is greater than or equal to 295 millimeters, then return Boolean values equal to 1, if not then 0
{
    if ( lin_pos >= LINEAR_MOVEMENT_STEP_MM - SHAFT_LENGTH_MM ) return 1;
    else                                                        return 0;
}


/*
 * @brief : Function converts the speed of motor [in revolutions per minute]
 *          into speed of carriage [ in millimeters per second].
 * @params: m_speed - spped of the motor [rpm]
 * @return: carriage speed [mm per sec]
 */
uint16_t mSpeed2linSpeed(uint16_t m_speed)
{
    return ( m_speed * LINEAR_MOVEMENT_STEP_MM ) / 60;
}

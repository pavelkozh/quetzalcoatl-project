#include <steer_control.h>
#include <lld_steer.h>
#include <feedback.h>
#include <pid.h>

static double steer_control_val = 0.0;
static float speed_ref = 0.0,
             prev_speed_ref = 0.0,
             steer_angle_ref = 0.0;
static bool steer_control_start = false;

 static PIDControllerContext_t  steer_pidCtx = {
    .kp   = 0.01,
    .ki   = 0.0,
    .kd   = 0.0,
    .integrLimit  = 100,
    .integZone = 0.5
};
static uint8_t CSErrorDeadzoneHalfwidth = 1;


static float position;
static float prev_position;
static int8_t position_cnt;
float steerPosControl( float steer_angle_ref ){

    float error = steer_angle_ref - position;

    /* Dead zone for (p) error */
    if ( abs(error) < CSErrorDeadzoneHalfwidth ){
        steer_pidCtx.err = 0;
    }
    else{
        steer_pidCtx.err = error;
    }
    steer_pidCtx.integZone_abs = steer_angle_ref * steer_pidCtx.integZone;
    steer_control_val    = PIDControlResponse( &steer_pidCtx );

    /*  roughly reset integral */
    steer_control_val = CLIP_VALUE(steer_control_val,-100.0,100.0);

    return steer_control_val;

}

void  steerPositionCalculate( void ){
        position = steerGetPosition();
        if(position_cnt == 0){
            if(position > 180) position -= 360;
        }else{
            position += position_cnt*360.0;
        }
        if(position - prev_position < -220) {
            position_cnt++;
            position+=360.0;
        }
        if(position - prev_position >  220) {
            position_cnt--;
            position-=360.0;
        }
        prev_position = position;
        //return position;
}
// -1 - error
// 0 - while calibration
// 1 - calibration finished OK
int8_t steerMotroDirCalibration( void ){
    uint8_t state_flag = 0;
        switch(){
            case 0:
                break;
        }
    steerMotorStartStopControl();

return 1;
}

static THD_WORKING_AREA(steer_pos_control_wa, 512);
static THD_FUNCTION(steer_pos_control, arg) {

    (void)arg;
    bool calibration_flag = 1;
    while(1){
        // palToggleLine(LINE_LED2);
        steerPositionCalculate();

        switch(calibration_flag){
            case 0:
                if(gazelGetEngineSpeed() > 700)
                    calibration_flag = steerMotroDirCalibration();
                break;
            case 1:
                if ( steer_control_start ){
                         speed_ref = steerPosControl( steer_angle_ref );
                     }
                else{
                    steer_pidCtx.err        = 0;
                    steer_pidCtx.prevErr    = 0;
                    steer_pidCtx.integrSum  = 0;
                    speed_ref = 0;
                    // //Motor stop if he run
                    // if(prev_position != position){
                    //     steerMotorStartStopControl();
                    //     steerMotorEnableInvert();
                    // }
                }
                if( ( (speed_ref<0) && (steerMotorGetDirection()!=0) ) || ((speed_ref>=0) && ( steerMotorGetDirection()!=1 )) ) steerMotorDirChange();
                if ( speed_ref < 0 ){
                    steerMotorSetSpeed(-speed_ref);
                }
                else{
                    steerMotorSetSpeed(speed_ref);
                }
                prev_speed_ref = speed_ref;
                break;
            default:
                break;
        }
        chThdSleepMilliseconds( 35 );
    }
}

static bool if_steer_control_module_initialized = false;

void steerInit(void) {

    if ( if_steer_control_module_initialized )
    {
        return;
    }
    
    PIDControlInit( &steer_pidCtx );
    
    
    
    steerEncInit();
    
    steerMotorInit();
    position = steerGetPosition();
    if(position > 180) position -= 360;
    position_cnt = 0;
    prev_position = position;

    chThdCreateStatic(steer_pos_control_wa, sizeof(steer_pos_control_wa), NORMALPRIO, steer_pos_control, NULL);

    
    if_steer_control_module_initialized = true;
    

}



void steerControlStart(void) {
    steer_control_start = true;
    if ( steerIsMotorEnable () == false ){
        steerMotorStartStopControl();
    }

}

void steerControlStop(void) {
    steer_control_start = false;
    if ( steerIsMotorEnable () == true ){
        steerMotorStartStopControl();
    }
   else{
       steer_pidCtx.err        = 0;
       steer_pidCtx.prevErr    = 0;
       steer_pidCtx.integrSum  = 0;
       speed_ref = steerGetPosition();
   }
}

void steerSetPosition ( float val ){
    if ( val > 180.0 ) val = 180.0;
    if ( val < -180.0 ) val = -180.0;
    steer_angle_ref = val;
}


float steerGetPos( void ){
    return position;
}


/**DBG functions**/

float steerDbgGetMotorCalcSpeedRef ( void )
{
    return speed_ref;
}

float steerDbgGetMotorCalcPosErr ( void )
{
    return steer_pidCtx.err;
}

float steerDbgGetMotorPosRef ( void )
{
    return steer_angle_ref;
}

bool steerDbgGetEnableFlag ( void )
{
    return steerIsMotorEnable ();
}

bool steerDbgGetDir( void )
{
    return steerMotorGetDirection();
}


int pos_cnt( void ){
    return position_cnt;
}

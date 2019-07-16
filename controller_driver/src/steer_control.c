#include <steer_control.h>
#include <lld_steer.h>
#include <pid.h>

static double steer_control_val = 0.0;
static float speed_ref = 0.0,
             prev_speed_ref = 0.0,
             steer_angle_ref = 0.0;
static bool steer_control_start = false;

static PIDControllerContext_t  steer_pidCtx = {
    .kp   = 1.0,
    .ki   = 0.0,
    .kd   = 0.0,
    .integrLimit  = 100,
    .integZone = 0.5
};
static uint8_t CSErrorDeadzoneHalfwidth = 1;



float steerPosControl( float steer_angle_ref ){

    float error = steer_angle_ref - steerGetPosition();

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


static THD_WORKING_AREA(steer_pos_control_wa, 256);
static THD_FUNCTION(steer_pos_control, arg) {

    (void)arg;

    while(1){

        if ( steer_control_start ){
            speed_ref = steerPosControl( steer_angle_ref );
        }
        else{
            steer_pidCtx.err        = 0;
            steer_pidCtx.prevErr    = 0;
            steer_pidCtx.integrSum  = 0;
            speed_ref = 0;
        }
        if ( sign(speed_ref) != sign(prev_speed_ref)){
            steerMotorDirChange();
        }
        steerMotorSetSpeed(speed_ref);

        prev_speed_ref = speed_ref;
        chThdSleepMilliseconds( 100 );
    }
}

static bool if_steer_control_module_initialized = false;

void steerInit(void) {

    if ( if_steer_control_module_initialized )
    {
        return;
    }

    PIDControlInit( &steer_pidCtx );
    chThdCreateStatic(steer_pos_control_wa, sizeof(steer_pos_control_wa), NORMALPRIO, steer_pos_control, NULL);
    steerEncInit();
    steerMotorInit();
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
}

void steerSetPosition ( float val ){
    steer_angle_ref = val;
}

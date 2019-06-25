#include <speed.h>
#include <pid.h>
#include <feedback.h>
#include <pedals.h>


static uint8_t     val = 57;  //57 = 1.1V
static float       Eref = 770.0;
static float       Vref = 0;


static int32_t VehicleControl = 0;
static int32_t engine_control_value =0;



static uint8_t CSErrorDeadzoneHalfwidth = 1;

static bool        engine_control_start = 0;
static bool        vehicle_control_start = 0;

static PIDControllerContext_t  pidCtx = {
    .kp   = 3.5,
    .ki   = 0.06,
    .kd   = 0,
    .integrLimit  = 5000,
    .integZone = 0.5
};

static PIDControllerContext_t  pidCtxV = {
    .kp   = 4,
    .ki   = 0.3,
    .kd   = 0,
    .integrLimit  = 100,
    .integZone = 0.9
};

/**
 * @brief    For synchronization
 * @param    engine_speed_rpm - reference [rpm]
 * @return   engine_control_value - GAS control [dac units]
 */
uint32_t speedEngineControl( uint32_t engine_speed_rpm )
{

    engine_speed_rpm = uint32_map(engine_speed_rpm,0,5000,0,5000);

    //engine_speed_rpm  = CLIP_VALUE( engine_speed_rpm, 0, 100 );

    int16_t current_engine_speed = uint32_map(gazelGetEngineSpeed (),0,5000,0,5000);

    int16_t error = engine_speed_rpm - current_engine_speed;

    /* Dead zone for (p) error */
    if ( abs(error) < CSErrorDeadzoneHalfwidth )
    {
        pidCtx.err = 0;
    }
    else
    {
        pidCtx.err = error;
    }
    pidCtx.integZone_abs = engine_speed_rpm * pidCtx.integZone;
    engine_control_value    = (int32_t) PIDControlResponse( &pidCtx );


    /*  roughly reset integral */
    engine_control_value = CLIP_VALUE(engine_control_value,0,5000);
    engine_control_value = uint32_map(engine_control_value,0,5000,0,100);

    return engine_control_value;

}

/**
 * @brief     Vehicle speed control function
 * @param     speed - reference [km/h]
 * @return    VehicleControl - GAS control [dac units]
 */

uint32_t speedVehicleControl( uint32_t speed )
{
    speed = uint32_map(speed,0,100,0,1000);
    int32_t current_speed = uint32_map(gazelGetSpeed(),0,100,0,1000);
    int32_t error = speed - current_speed;

    /* Dead zone for (p) error */
    if ( abs(error) < CSErrorDeadzoneHalfwidth )
    {
        pidCtxV.err = 0;
    }
    else
    {
        pidCtxV.err = error;
    }
    pidCtxV.integZone_abs = speed * pidCtxV.integZone;
    VehicleControl    = (int32_t) PIDControlResponse( &pidCtxV );


    /*  roughly reset integral */
    VehicleControl = CLIP_VALUE(VehicleControl,0,1000);
    VehicleControl = uint32_map(VehicleControl,0,1000,0,100);

    return VehicleControl;

}
static THD_WORKING_AREA(pid_wa, 256);
	static THD_FUNCTION(pid, arg) {

    (void)arg;
    palSetLine(LINE_LED3);
    bool CBflag =0;

    while(1){

        if(( gazelGetBrakeSwitch() == 1 ) || ( gazelGetClutchSwitch() == 1 ))
            CBflag = 0;
        else
            CBflag = 1;



        if ( (engine_control_start )  && ( !(vehicle_control_start && CBflag) ) ){
            val = speedEngineControl(( uint32_t ) Eref);
        }
        else{
                pidCtx.err        = 0;
                pidCtx.prevErr    = 0;
                pidCtx.integrSum  = 0;
                engine_control_value = 0;
        }



        if ( (vehicle_control_start) && (!engine_control_start ) && (CBflag) ){
            palToggleLine(LINE_LED1);
            val = speedVehicleControl((uint32_t) Vref);
        }
        else{
                pidCtxV.err        = 0;
                pidCtxV.prevErr    = 0;
                pidCtxV.integrSum  = 0;
                VehicleControl = 0;
        }


        if( (!engine_control_start ) && (!(vehicle_control_start && CBflag)) )
        {
            val = 0;
        }


        pedalsAcceleratorControl ( val );

        if(engine_control_start)    chThdSleepMilliseconds( 20 );
        else                         chThdSleepMilliseconds( 100 );

    }
}
void speedInit(void) {
	PIDControlInit( &pidCtxV );
    PIDControlInit( &pidCtx );
	chThdCreateStatic(pid_wa, sizeof(pid_wa), NORMALPRIO, pid, NULL);
	pedalsInit();

}

void speedEngineControlStart(void) {
	engine_control_start = true;
}

void speedEngineControlStop(void) {
	engine_control_start = false;
}

void speedVehicleControlStart(void) {
	vehicle_control_start = true;
}

void speedVehicleControlStop(void) {
	vehicle_control_start = false;
}

void speedSetVehiclePIDReferenceValue ( float val ){
    Vref = val;
}

void speedSetEnginePIDReferenceValue ( float val ){
    Eref = val;
}



/************** DEBUG FUNCTIONS********************/
float speedGetVehicleReference (void)
{
    return Vref;
}

float speedGetEngineReference (void)
{
    return Eref;
}

bool speedGetVehicleControlFlag (void)
{
    return vehicle_control_start;
}

bool speedGetEngineControlFlag (void)
{
    return engine_control_start;
}

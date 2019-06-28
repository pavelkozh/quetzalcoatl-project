/* all functions names in this module starts with "hsc" -
 * which stands for "High Speed Control" */


#include <high_speed_control.h>
#include <speed.h>
#include <pid.h>
#include <MT_control.h>
#include <feedback.h>
#include <pedals.h>


/* GLOBAL VARIABLES */
static int8_t vehicle_speed_ref = 20; // call usb function
static double vehicle_speed = 0.0,
       vehicle_speed_err = 0.0;
static bool hsc_thd_sleep = false;


/* FUNCTIONS */

static thread_reference_t trp_hsc = NULL;

void hscStart ( void )
{
    mtControlStart();
    speedVehicleControlStart();

    // resume hsc (high speed control module wake up)
    chSysLock();
    chThdResume(&trp_hsc, MSG_OK);
    chSysUnlock();

    // resume mt_control (mt_control.c)
    // resume vehicle control (speed.c)


}

void hscStop ( void )
{
    mtControlStop();
    speedVehicleControlStop();
    speedEngineControlStop(); //???

    /* mt_control module goes to sleep until call (resume) */
    hsc_thd_sleep = true;

    // where (in which .c file) create flag?





}


#define SpeedErrorDeadZoneWidth    0.1 //km/h
static uint32_t brake_control_value = 0;
static PIDControllerContext_t  pidCtxBrake = {
    .kp   = 10,
    .ki   = 0.0,
    .kd   = 0.0,
    .integrLimit  = 5000,
    .integZone = 1.0
};

uint32_t brakeControl( void )
{
    /* Dead zone for (p) error */
    if ( vehicle_speed_err >= -SpeedErrorDeadZoneWidth )
    {
        pidCtxBrake.err = 0;
    }
    else
    {
        pidCtxBrake.err = vehicle_speed_err;
    }
    brake_control_value    = (int32_t) PIDControlResponse( &pidCtxBrake );


    brake_control_value = CLIP_VALUE(brake_control_value,0,100);
    //brake_control_value = uint32_map(brake_control_value,0,1000,0,40000); //use MAX_BRAKE_POS!!! create function in pedals.c

    return brake_control_value;
}

//static thread_reference_t trp_hsc_brake = NULL;

static THD_WORKING_AREA(hsc_brake_wa, 256);
static THD_FUNCTION(hsc_brake, arg) {
    (void)arg;
    uint32_t brake_force = 0;

    while(1){


        brake_force =  brakeControl();
        if ( brake_force == 0 ) //??  press brake pedal while stop ??
        {
            pedalsBrakeRelease(1000);
        }
        else
        {
            brake_force = uint32_map(brake_force,0,100,10000,500);
            pedalsBrakePress(brake_force);
        }

        chThdSleepMilliseconds( 100 );
    }
}





static THD_WORKING_AREA(hsc_wa, 256);
static THD_FUNCTION(hsc, arg) {
    (void)arg;

    while(1){
        vehicle_speed = gazelGetSpeed(); // while test

        // call usb function !!!! vehicle_speed_ref = some_fun();..
        //speedSetVehiclePIDReferenceValue ((float) vehicle_speed_ref);

        vehicle_speed_err = vehicle_speed_ref - vehicle_speed;
        if ( ( vehicle_speed_err <= -5 ) || ( vehicle_speed_ref == 0 ) )
        {
            pedalsClutchPress( 1000 );
        }
        else if (( vehicle_speed_err >= -5 ) && ( vehicle_speed_err < 0 ) && ( vehicle_speed_ref != 0 ) )
        {
            // check gear
            pedalsClutchRelease( 2000 );
        }

        if ( hsc_thd_sleep )
        {
            /* high speed control module goes to sleep until call (resume) */
            chSysLock();
            chThdSuspendS(&trp_hsc);
            chSysUnlock();
        }


        chThdSleepMilliseconds( 100 );

    }
}


void hscInit ( void )
{
    mtControlInit (); // two threads which controls gearshifing start working!
    feedbackInit();
    speedInit(); // acceleration control thread starts working!
    pedalsInit();
    chThdCreateStatic(hsc_brake_wa, sizeof(hsc_brake_wa), NORMALPRIO + 7, hsc_brake, NULL);
    chThdCreateStatic(hsc_wa, sizeof(hsc_wa), NORMALPRIO + 7, hsc, NULL);
}


/// test functions!!!!
void hscTestSetSpeedRef (int8_t speed)
{
    vehicle_speed_ref = speed;
}

void hscTestSetCurrentSpeed ( int8_t speed )
{
    vehicle_speed = speed;
}






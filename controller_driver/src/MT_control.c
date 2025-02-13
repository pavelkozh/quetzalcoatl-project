#include <MT_control.h>
#include <MT.h>
#include <pedals.h>
#include <feedback.h>
#include <speed.h>



#define ENGINE_SPEED_THRESHOLD 1500 // engine speed, when gear shifting occur

static thread_reference_t trp_gearshift = NULL;



static int8_t gear_num = 0; // currently engaged gear ("feedback")
static uint8_t gear = -1;   // command gear
static bool gear_shift_control = 0; // Gearbox control enable flag
static bool shift_enable_flag = 0;  // Gearshifting enable flag (service variable)



//

static THD_WORKING_AREA(gearshift_wa, 256);
static THD_FUNCTION(gearshift, arg) {

    (void)arg;
    while(1){


        switch(gear){
        case 0: gear_num = shiftMTToNeutral ( 2000 ); break;
        case 1: gear_num = shiftMTToNextGear(1,2000); break;
        case 2: gear_num = shiftMTToNextGear(2,5000); break;
        case 3: gear_num = shiftMTToNextGear(3,10000); break;
        case 4: gear_num = shiftMTToNextGear(4,10000); break;
        case 5: gear_num = shiftMTToNextGear(5,10000); break;
        case 6: gear_num = shiftMTToNextGear(6,2000); break;
        }

        /* Shifting is finish and thread go to sleep! */
        if (gear_num == gear)
        {
            chSysLock();
            chThdSuspendS(&trp_gearshift);
            chSysUnlock();
        }

        chThdSleepMilliseconds( 20 );
    }
}





static THD_WORKING_AREA(mt_control_wa, 256);
static THD_FUNCTION(mt_control, arg) {

    (void)arg;
    while(1){
            if ( ( gazelGetEngineSpeed () >= ENGINE_SPEED_THRESHOLD ) && (gear_shift_control == 1) && ( gear_num != 2 ) )
            {
                //palToggleLine(LINE_LED2);
                shift_enable_flag = 1;
            }

            if ( shift_enable_flag )
            {
                if ( gear_num != 2  )
                {
                    pedalsClutchPress ( 500 );
                    if ( pedalsClutchGetState () == 0 )
                    {
                        /* Shifting is start and thread wake up! */
                        chSysLock();
                        chThdResumeS(&trp_gearshift, MSG_OK);
                        chSysUnlock();

                        gear = 2;

                        /*K1 = 149; K2 = 75.3; K3 = 49*/
                        speedSetEnginePIDReferenceValue( 17* 75.3);//gazelGetSpeed() * 75.3 );
                        speedEngineControlStart();
                    }
                }
                if ( gear_num == 2  )
                {
                    pedalsClutchRelease (500);
                    if ( pedalsClutchGetState () == 0 )
                    {
                        shift_enable_flag = 0;
                        speedEngineControlStop();
                        //speedVehicleControlStart();  //////////????????????
                    }
                }
            }

            chThdSleepMilliseconds( 20 );
    }


}

void mtControlStart ( void ) {
    gear_shift_control = true;
}

void mtControlStop( void ) {
    gear_shift_control = false;
}

bool mtControlGetEnableFlag ( void ){
    return gear_shift_control;
}


int8_t mtControlMannualyShiftGear ( uint8_t command_gear )
{

    /* Shifting is start and thread wake up! */
    gear = command_gear;
    chThdResume(&trp_gearshift, MSG_OK);


    return gear_num;
}

static bool if_mtControl_module_initialized = 0;

/*
 * @brief    Initialization of manual transmition controller
 */
void mtControlInit ( void )
{
    if ( if_mtControl_module_initialized )
    {
        return;
    }


    mtMotorsControlInit ();
    chThdCreateStatic(gearshift_wa, sizeof(gearshift_wa), NORMALPRIO, gearshift, NULL);
    chThdCreateStatic(mt_control_wa, sizeof(mt_control_wa), NORMALPRIO, mt_control, NULL);
    if_mtControl_module_initialized = 1;
}

int8_t mtControlGetCurrentGearNum (void)
{
    return gear_num;
}

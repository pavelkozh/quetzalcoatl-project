#include <MT_control.h>


extern  gazelParam gazel;




/************************** MT Control *********/

#define ENGINE_SPEED_THRESHOLD 1900 // engine speed, when gear shifting occur

bool gear_shift_control = 0;
int8_t gear_num = 0;
uint8_t gear = -1;
float eng_speed_debug = 0;
bool shift_enable_flag = 0;

static THD_WORKING_AREA(mt_control_wa, 256);
static THD_FUNCTION(mt_control, arg) {

    (void)arg;
    while(1){
        switch(gear){
        case 0: gear_num = shiftMTToNeutral(1000); break;
        case 1: gear_num = shiftMTToNextGear(1,1000); break;
        case 2: gear_num = shiftMTToNextGear(2,1000); break;
        case 6: gear_num = shiftMTToNextGear(6,1000); break;
        }

        /* Shifting is finish and thread go to sleep! */
        if (gear_num == gear)
        {
            chSysLock();
            chThdSuspendS(&mt_control);
            chSysUnlock();
        }

        chThdSleepMilliseconds( 20 );
    }
}





static THD_WORKING_AREA(gearshift_wa, 256);
static THD_FUNCTION(gearshift, arg) {

    (void)arg;
    while(1){
            if ( ( gazel.EngineSpeed >= ENGINE_SPEED_THRESHOLD ) && (gear_shift_control == 1) && ( gear_num != 2 ) )
            {
                shift_enable_flag = 1;
            }

            if ( shift_enable_flag )
            {
                if ( gear_num != 2  )
                {
                    pressClutchPedal ( 500 );
                    if ( getClutchPedalState () == 0 )
                    {
                        /* Shifting is start and thread wake up! */
                        chSysLock();
                        chThdResume(&mt_control, MSG_OK);
                        chSysUnlock();

                        gear = 2;

                        /*K1 = 149; K2 = 75.3; K3 = 49*/
                        Eref = gazel.Speed * 75.3;
                        engine_control_start = 1;
                    }
                }
                if ( gear_num == 2  )
                {
                    releaseClutchPedal (500);
                    if ( getClutchPedalState () == 0 )
                    {
                        shift_enable_flag = 0;
                        engine_control_start = false;
                        vehicle_control_start = true;
                    }
                }
            }

            chThdSleepMilliseconds( 20 );
    }


}



/*
 * @brief    Initialization of manual transmition controller
 */
void MTControlInit ( void )
{
    mtControlInit ();
    chThdCreateStatic(gearshift_wa, sizeof(gearshift_wa), NORMALPRIO + 7, gearshift, NULL);
    chThdCreateStatic(mt_control_wa, sizeof(mt_control_wa), NORMALPRIO + 7, mt_control, NULL);
}








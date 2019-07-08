/* all functions names in this module starts with "hsc" -
 * which stands for "High Speed Control" */


#include <high_speed_control.h>
#include <speed.h>
//#include <pid.h>
#include <MT_control.h>
#include <feedback.h>
#include <pedals.h>


/* GLOBAL VARIABLES */
/////


/* FUNCTIONS */

void hscInit ( void )
{
    feedbackInit();
    pedalsInit();
    mtControlInit (); // two threads which controls gearshifing start working!
    speedInit(); // acceleration control thread starts working!
}

void hscStart ( void )
{
    mtControlStart();
    speedVehicleControlStart();


    // resume mt_control (mt_control.c)
    // resume vehicle control (speed.c)

}

void hscStop ( void )
{
    mtControlStop();
    speedVehicleControlStop();
    speedEngineControlStop(); //???

    /* mt_control module goes to sleep until call (resume) */
    // where (in which .c file) create flag?

}









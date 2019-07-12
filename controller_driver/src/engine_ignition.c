#include <engine_ignition.h>
#include <feedback.h>

#define IGNITION_PAL_LINE  PAL_LINE(GPIOF,12)
#define STARTER_PAL_LINE   PAL_LINE(GPIOF,13)

/* initialization flag - used for control that Init function execute once */
bool if_eng_ignition_tnitialized = false;
void engIgnitionInit ( void )
{
    if ( if_eng_ignition_tnitialized )
    {
        return;
    }
    palSetLineMode( IGNITION_PAL_LINE, PAL_MODE_OUTPUT_PUSHPULL );
    palSetLineMode( STARTER_PAL_LINE,  PAL_MODE_OUTPUT_PUSHPULL );

    feedbackInit();
    //chCreateStatic (); // if thread is needed

    if_eng_ignition_tnitialized = true;
}

void engIgnitionSwitchOn ( void )
{
    palSetLine(IGNITION_PAL_LINE);

    while ( gazelGetEngineSpeed() < 730 )
    {
        palSetLine(STARTER_PAL_LINE);
    }
    palClearLine(STARTER_PAL_LINE);

}

void engIgnitionSwitchOff ( void )
{
    palClearLine(IGNITION_PAL_LINE);


}

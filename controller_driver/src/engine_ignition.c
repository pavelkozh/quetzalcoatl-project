#include <engine_ignition.h>
#include <feedback.h>

#define IGNITION_PAL_LINE  PAL_LINE(GPIOF,12)
#define STARTER_PAL_LINE   PAL_LINE(GPIOF,13)

static thread_reference_t trp_eng_ignition = NULL;
static bool is_engine_start_thd_working = false;

static THD_WORKING_AREA(eng_ignition_wa, 256);
static THD_FUNCTION(eng_ignition, arg) {
    (void)arg;

    while(1){
        if ( is_engine_start_thd_working == false ){
            chSysLock();
            chThdSuspendS(&trp_eng_ignition);
            chSysUnlock();
        }
        else{
            palSetLine(LINE_LED1);
            palSetLine(IGNITION_PAL_LINE);
            palSetLine(STARTER_PAL_LINE);
            while ( gazelGetEngineSpeed() < 730 ){
                palSetLine(LINE_LED2);
                chThdSleepMilliseconds( 50 );
            }
            palClearLine(STARTER_PAL_LINE);
            palSetLine(LINE_LED3);

            /* starting is finish -> thread goes to sleep*/
            is_engine_start_thd_working == false;
            chSysLock();
            chThdSuspendS(&trp_eng_ignition);
            chSysUnlock();

        }
        chThdSleepMilliseconds( 50 );
    }

}


/* initialization flag - used for control that Init function execute once */
static bool if_eng_ignition_tnitialized = false;
void engIgnitionInit ( void )
{
    if ( if_eng_ignition_tnitialized )
    {
        return;
    }

    /* leds for debugging */
    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );   //Led


    palSetLineMode( IGNITION_PAL_LINE, PAL_MODE_OUTPUT_PUSHPULL );
    palSetLineMode( STARTER_PAL_LINE,  PAL_MODE_OUTPUT_PUSHPULL );

    feedbackInit();
    chThdCreateStatic(eng_ignition_wa, sizeof(eng_ignition_wa), NORMALPRIO + 1, eng_ignition, NULL);

    if_eng_ignition_tnitialized = true;
}

bool engIgnitionSwitchOn ( void )
{
    if (is_engine_start_thd_working == false) {
        is_engine_start_thd_working = true;
        /* Wake up engine start thread */
        chSysLock();
        chThdResume(&trp_eng_ignition, MSG_OK);
        chSysUnlock();
    }
    return  ( is_engine_start_thd_working && ( gazelGetEngineSpeed() > 750 ) );

}

void engIgnitionSwitchOff ( void )
{
    palClearLine(IGNITION_PAL_LINE);


}

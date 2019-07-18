#include <engine_ignition.h>
#include <feedback.h>

#define IGNITION_PAL_LINE  PAL_LINE(GPIOF,12)
#define STARTER_PAL_LINE   PAL_LINE(GPIOF,13)

static uint16_t dbgVal = 0;

static thread_reference_t trp_eng_ignition = NULL;
static uint8_t is_engine_start_thd_working = 0;

static THD_WORKING_AREA(eng_ignition_wa, 512);
static THD_FUNCTION(eng_ignition, arg) {
    (void)arg;

    while(1){
        if ( is_engine_start_thd_working == 0 ){
            chSysLock();
            chThdSuspendS(&trp_eng_ignition);
            chSysUnlock();
        }
        else{

            palClearLine(IGNITION_PAL_LINE);
            palClearLine(STARTER_PAL_LINE);
            while ( dbgVal < 730 ){ //( gazelGetEngineSpeed() < 730 ){
                palSetLine(LINE_LED2);
                chThdSleepMilliseconds( 50 );
            }
            palSetLine(STARTER_PAL_LINE);
            palClearLine(LINE_LED2);


            /* starting is finish -> thread goes to sleep*/
            is_engine_start_thd_working = 0;
            palSetLine(LINE_LED3);
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

    palSetLine(IGNITION_PAL_LINE);
    palSetLine(STARTER_PAL_LINE);

    feedbackInit();
    chThdCreateStatic(eng_ignition_wa, sizeof(eng_ignition_wa), NORMALPRIO + 1, eng_ignition, NULL);

    if_eng_ignition_tnitialized = true;
}

bool engIgnitionSwitchOn ( void )
{
    if (is_engine_start_thd_working == 0) {
        is_engine_start_thd_working = 1;
        palSetLine(LINE_LED1);
        /* Wake up engine start thread */
        chSysLock();
        chThdResume(&trp_eng_ignition, MSG_OK);
        chSysUnlock();
    }
    return  ( is_engine_start_thd_working && ( gazelGetEngineSpeed() > 750 ) );

}

void engIgnitionSwitchOff ( void )
{
    palSetLine(IGNITION_PAL_LINE);

    palClearLine(LINE_LED1);
    palClearLine(LINE_LED2);
    palClearLine(LINE_LED3);
}

void engIgnitionDbgSetEngSpeed ( uint16_t val )
{
    dbgVal = val;
}

uint8_t engIgnitionDbgGetThreadEnFlag ( void )
{
    return is_engine_start_thd_working;
}


uint8_t engIgnitionDbgGetFlaf ( void )
{

    return 12;
}

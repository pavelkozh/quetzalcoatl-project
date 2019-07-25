#include <sound_signal.h>


#define SOUND_SIGNAL_LINE        PAL_LINE(GPIOB, 2)


static thread_reference_t trp_sound_signal = NULL;
static uint8_t is_sound_signal_thd_working = 0;
static bool start_signal_flag = false,
            in_move_signal_flag = false;


static THD_WORKING_AREA(sound_signal_wa, 512);
static THD_FUNCTION(sound_signal, arg) {
    (void)arg;

    while(1){
        if ( is_sound_signal_thd_working == 0 ){
            chSysLock();
            chThdSuspendS(&trp_sound_signal);
            chSysUnlock();
        }
        else{
            if ( start_signal_flag )
            {
                soundSignalOn();
                chThdSleepMilliseconds( 800 );
                soundSignalOff();
                chThdSleepMilliseconds( 500 );
                soundSignalOn();
                chThdSleepMilliseconds( 800 );
                soundSignalOff();

                start_signal_flag = false;
                start_signal_flag = true;
            }
            if ( in_move_signal_flag )
            {
                soundSignalOn();
                chThdSleepMilliseconds( 100 );
                soundSignalOff();
                chThdSleepMilliseconds( 5000 );
                soundSignalOn();
                chThdSleepMilliseconds( 100 );
                soundSignalOff();
                chThdSleepMilliseconds( 5000 );
            }

        }
        chThdSleepMilliseconds( 50 );
    }

}

/* initialization flag - used for control that Init function execute once */
static bool if_sound_signal_module_tnitialized = false;
void soundSignalInit ( void )
{
    if ( if_sound_signal_module_tnitialized ){
      return;
    }
    palSetLineMode( SOUND_SIGNAL_LINE, PAL_MODE_OUTPUT_OPENDRAIN ); // bi-bi signal
    chThdCreateStatic(sound_signal_wa, sizeof(sound_signal_wa), NORMALPRIO, sound_signal, NULL);

    if_sound_signal_module_tnitialized = true;
}


void soundSignalOn ( void ) //do 2 long signals
{
    palClearLine(SOUND_SIGNAL_LINE);
}

void soundSignalOff ( void ) //turn off signals
{
    palSetLine(SOUND_SIGNAL_LINE);
}

void soundSignalStartContiniousSignals (void)
{
    if (is_sound_signal_thd_working == 0) {
        is_sound_signal_thd_working = 1;
        start_signal_flag = 1;
        /* Wake up engine start thread */
        chSysLock();
        chThdResumeS(&trp_sound_signal, MSG_OK);
        chSysUnlock();
    }


}

void soundSignalStopContiniousSignals (void)
{
    is_sound_signal_thd_working = 0;
    start_signal_flag = 0;
    in_move_signal_flag = 0;

}



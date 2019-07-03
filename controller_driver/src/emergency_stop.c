#include <emergency_stop.h>
#include <pedals.h>

#define EMERGENCY_STOP_LINE PAL_LINE( GPIOA, 0 )

static thread_reference_t trp_emergency_button_stop = NULL;

bool flag = false;

static THD_WORKING_AREA(emergency_button_stop_wa, 256);
static THD_FUNCTION(emergency_button_stop, arg) {
    (void)arg;
    while(1){
        if ( flag == false )
        {
            chSysLock();
            chThdSuspendS(&trp_emergency_button_stop);
            chSysUnlock();
        }
        else
        {
            pedalsClutchPress ( 1000 );
            if ( pedalsClutchGetState () == false )
            {
              pedalsBrakePress( 5000 );
              if ( pedalsBrakeGetState() == false )
              {
                  // gear = neutral!
                  // zagiganie off!!!
                  flag = false;
                  chSysLock();
                  chThdSuspendS(&trp_emergency_button_stop);
                  chSysUnlock();
              }
            }
        }

        chThdSleepMilliseconds( 1 );
    }
}

void extcb_base(EXTDriver *extp, expchannel_t channel)
{
    palToggleLine(LINE_LED1);
    flag = true;
    chSysLock();
    chThdResume(&trp_emergency_button_stop, MSG_OK);
    chSysUnlock();
    (void)extp;
    (void)channel;
}

static bool isInitialized = false;

void emergencyStopInit( void ) {

	if ( isInitialized )
		return;
    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );   //Led

    pedalsInit();
    chThdCreateStatic(emergency_button_stop_wa, sizeof(emergency_button_stop_wa), NORMALPRIO + 15, emergency_button_stop, NULL);
	palSetLineMode( PAL_LINE(GPIOA,0), PAL_MODE_INPUT_PULLUP);
	commonExtDriverInit();
	EXTChannelConfig base_conf = {
	    .mode = EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOA, 
	    .cb = extcb_base // имя обработчика
	};

	extSetChannelMode( &EXTD1, 0, &base_conf ); // 0 - номер пина, порт раньше



	isInitialized = true;
}

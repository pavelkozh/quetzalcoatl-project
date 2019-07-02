#include <emergency_stop.h>
#include <pedals.h>

#define EMERGENCY_STOP_LINE PAL_LINE( GPIOA, 0 )


void extcb_base(EXTDriver *extp, expchannel_t channel)
{
    palToggleLine(LINE_LED1);
    pedalsClutchPress ( 1000 );
    if ( pedalsClutchGetState () == false ) {
    	pedalsBrakePress( 5000 );
    }
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
	palSetLineMode( PAL_LINE(GPIOA,0), PAL_MODE_INPUT_PULLUP);
	commonExtDriverInit();
	EXTChannelConfig base_conf = {
	    .mode = EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOA, 
	    .cb = extcb_base // имя обработчика
	};

	extSetChannelMode( &EXTD1, 0, &base_conf ); // 0 - номер пина, порт раньше

	isInitialized = true;
}
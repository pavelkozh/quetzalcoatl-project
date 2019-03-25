#include <lld_dac.h>

static const DACConfig dac_conf = {
	    /* Initial value of DAC out */
	    .init         =  0,
	    /* Mode of DAC */
	    .datamode     = DAC_DHRM_12BIT_RIGHT,
	    /* Direct register set, future used for triggering DAC */
	    .cr = 0
};


void dac_init( void ) {
    /* Set pin to analogue mode */
    palSetLineMode( PAL_LINE( GPIOA, 4 ), PAL_MODE_INPUT_ANALOG );

    /* Start DAC driver with configuration */
    dacStart( &DACD1, &dac_conf );

    /*   Put value	 */
  //  dacPutChannelX( &DACD1, 0 ,VOLTAGE_2_DAC(2) );
};

#include <tests.h>
#include <tf_calculation.h>
#include <chprintf.h>
#include <lld_sensor_module.h>
#include <lld_dac.h>
#include <term_graph.h>

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

/* Transfer function state initialization*/
TFConf_t tfconf = {
                   .k             = 1,
                   .T             = 0.1,
                   .prev_output   = 0.0,
                   .output        = 0.0,
                   .input         = 0,
				   .a             = 0,
				   .b             = 0
};

/* pointer to stucture*/
TFConf_t *p_tfconf = &tfconf;


uint16_t gpt_callback_counter = 0, i = 0;
float out_array[10000] = {0.0};
uint32_t cnt = 0;
bool start = 0;

/* Difference equation calculation on timer trigger event (overflow)
 * Ones in (time interval = 500 times * timer period) calculated value
 * will recorded to array.
 */
static void gpt3_callback (GPTDriver *gptp)
{
    (void)gptp;
    //tfconf.input = palReadPad(GPIOC, 13);//commonADC1UnitGetValue ( 2 );
    tfOutCalculation ( p_tfconf );
    dacPutChannelX( &DACD1, 0 ,(int32_t)tfconf.output);
    gpt_callback_counter ++;
    if ( start == 1 )
    {
            out_array[cnt] = tfconf.output;
            cnt++;
    }

}

/*
 * GPT3 configuration. This timer is used as trigger for the transfer function
 * calculation.
 */
static const GPTConfig gpt3cfg1 = {
  .frequency =  1000000U,        /*  1MHz*/
  .callback  =  gpt3_callback,
  .cr2       =  TIM_CR2_MMS_1,  /* MMS = 010 = TRGO on Update Event.        */
  .dier      =  0U
};

void testTFCalcRouting ( void )
{
	sdStart( &SD3, &sdcfg );
	palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) ); // TX
	palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) ); // RX

	//Button
	palSetPadMode(GPIOC, 13,PAL_MODE_INPUT);

	//Sensor init. 1 - PC0, 2 - PA3
    commonADC1UnitInit();

    //DAC init. PA4 (ch1)
    dac_init();

    /* Starting GPT3 driver */
    gptStart(&GPTD3, &gpt3cfg1);
    gptStartContinuous(&GPTD3, gpt3cfg1.frequency / 1000); //Timer period = 1ms

    uint8_t sd_buff[5];

    while ( 1 )
    {
    sdReadTimeout( &SD3, sd_buff, 5, TIME_IMMEDIATE);

     if(sd_buff[4]=='s'){
    	 start = 1;
    	 cnt = 0;
    	 tfconf.input = atoi(sd_buff);
    	 chprintf( (BaseSequentialStream *)&SD3, "y = %d \n\r",(int32_t) (tfconf.output)); //k= %d\t T = %d,ms\t  input = %d\t prev_out = %d\t
     }else{
    	 if(start==1) sdWrite(&SD3, &out_array , cnt );
    	 start=0;
     }
     chThdSleepMilliseconds( 10 );
    }
}

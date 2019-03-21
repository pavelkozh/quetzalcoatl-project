#include <tests.h>
#include <tf_calculation.h>
#include <chprintf.h>

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

/* Transfer function state initialization*/
TFConf_t tfconf = {
                   .k             = 1023,
                   .T             = 1.0,
                   .prev_output   = 0.0,
                   .output        = 0.0,
                   .input         = 0,
				   .a             = 0,
				   .b             = 0
};

/* pointer to stucture*/
TFConf_t* p_tfconf = &tfconf;


uint16_t gpt_callback_counter = 0, i = 0;
float out_array[205] = {0.0};

/* Difference equation calculation on timer trigger event (overflow)
 * Ones in (time interval = 500 times * timer period) calculated value
 * will recorded to array.
 */
static void gpt3_callback (GPTDriver *gptp)
{
    (void)gptp;
    tfOutCalculation ( p_tfconf );
    gpt_callback_counter ++;
    if ( gpt_callback_counter == 500 )
    {
        gpt_callback_counter = 0;
        if ( i <= 204 )
        {
            out_array[i] = tfconf.output;
            i++;
        }


    }

}

/*
 * GPT3 configuration. This timer is used as trigger for the transfer function
 * calculation.
 */
static const GPTConfig gpt3cfg1 = {
  .frequency =  100000U,        /* 100 kHz*/
  .callback  =  gpt3_callback,
  .cr2       =  TIM_CR2_MMS_1,  /* MMS = 010 = TRGO on Update Event.        */
  .dier      =  0U
};

void testTFCalcRouting ( void )
{
    sdStart( &SD7, &sdcfg );
    palSetPadMode( GPIOE, 8, PAL_MODE_ALTERNATE(8) );   // TX
    palSetPadMode( GPIOE, 7, PAL_MODE_ALTERNATE(8) );   // RX

    /* Starting GPT3 driver */
    gptStart(&GPTD3, &gpt3cfg1);
    gptStartContinuous(&GPTD3, gpt3cfg1.frequency / 1000); //Timer period = 1ms


    while ( 1 )
    {
      chprintf( (BaseSequentialStream *)&SD7, "k= %d\t T = %d\t input = %d\t prev_out = %d\t y = %d\n\r",
                (uint8_t) (tfconf.k * 10),
                (uint16_t) (tfconf.T * 10),
                (uint16_t) (tfconf.input * 10),
                (int32_t) (tfconf.prev_output * 10),
                (int32_t) (out_array[i] * 1000) );
      //(int32_t) (tfconf.output * 1000))

       // chprintf( (BaseSequentialStream *)&SD7, "y = :  %d\r\n", (int32_t) (tfconf.output * 10000) );
        chThdSleepMilliseconds( 500 );

    }
}

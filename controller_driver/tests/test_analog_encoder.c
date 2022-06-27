#include <tests.h>
#include <chprintf.h>

#include <pedals.h>
#include <feedback.h>

#define ADC3_NUM_CHANNELS   1
#define ADC3_BUF_DEPTH      1

uint16_t adc_res=0;

static adcsample_t samples1[ADC3_NUM_CHANNELS * ADC3_BUF_DEPTH];

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

/*
 * GPT4 configuration. This timer is used as trigger for the ADC.
 */
static const GPTConfig gpt5cfg1 = {
  .frequency =  100000,
  .callback  =  NULL,
  .cr2       =  TIM_CR2_MMS_1,  /* MMS = 010 = TRGO on Update Event.        */
  .dier      =  0U
  /* .dier field is direct setup of register, we don`t need to set anything here until now */
};

/* ADC streaming callback */
static void adccallback(ADCDriver *adcp, adcsample_t *buffer, size_t n)
{
  (void)adcp;
  (void)buffer;
  (void)n;
  palToggleLine(LINE_LED2);
  adc_res=buffer[0];


}

/* ADC errors callback, should never happen */
static void adcerrorcallback(ADCDriver *adcp, adcerror_t err) {

  (void)adcp;
  (void)err;
  palSetLine(LINE_LED3);

}

static const ADCConversionGroup adcgrpcfg1 = {
  .circular     = true,                                           // working mode = looped
  .num_channels = ADC3_NUM_CHANNELS,
  .end_cb       = adccallback,
  .error_cb     = adcerrorcallback,
  .cr1          = 0,
  .cr2          = ADC_CR2_EXTEN_RISING | ADC_CR2_EXTSEL_SRC(0b0100),  // Commutated from GPT
  .smpr1        = ADC_SMPR1_SMP_AN14(ADC_SAMPLE_144),             // for AN10 - 144 samples
  .smpr2        = 0,              // for AN3  - 144 samples
  .sqr1         = ADC_SQR1_NUM_CH(ADC3_NUM_CHANNELS),
  .sqr2         = 0,
  .sqr3         = ADC_SQR3_SQ1_N(ADC_CHANNEL_IN14)
  /* If we can macro ADC_SQR2_SQ... we need to write to .sqr2 */
};




void testAnalogEncoder ( void )
{


    gptStart(&GPTD5, &gpt5cfg1);
    gptStartContinuous(&GPTD5, gpt5cfg1.frequency/300);

    // ADC driver
    adcStart(&ADCD3, NULL);
    palSetLineMode( PAL_LINE(GPIOF, 4), PAL_MODE_INPUT_ANALOG );  // PA1
    adcStartConversion(&ADCD3, &adcgrpcfg1, samples1, ADC3_BUF_DEPTH);

    // how often we need ADC value
    /* Just set the limit (interval) of timer counter, you can use this function
       not only for ADC triggering, but start infinite counting of timer for callback processing */


    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX


    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );   //Led

    chprintf( (BaseSequentialStream *)&SD3, "Start\n\r");

    while(1) {


        palToggleLine(LINE_LED1);
        chprintf( (BaseSequentialStream *)&SD3, "res :%d\n\r",adc_res);
        chThdSleepMilliseconds( 10 );
    }
}













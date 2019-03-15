#include <lld_encoder.h>
#include <lld_encoder.h>

/***    ENCODER CONFIGURATION ***/
/* Specify encoder impulses quantity per revolution*/
#define ENC_MAX_TICK_NUM        1000  // 1000 * 2 (for BOTH_EDGES)

/* Timeout for velocity calculation */
const uint32_t  encoderTimeoutMs   = 1000;



/*******************************/
/***    LINE CONFIGURATION   ***/
/*******************************/
#define ENCODER_CH_A_PAD    7
#define ENCODER_CH_B_PAD    8
#define ENCODER_NULL_PAD    9

#define ENCODER_CH_A_LINE   PAL_LINE( GPIOF, ENCODER_CH_A_PAD )
#define ENCODER_CH_B_LINE   PAL_LINE( GPIOF, ENCODER_CH_B_PAD )
#define ENCODER_NULL_LINE   PAL_LINE( GPIOF, ENCODER_NULL_PAD )
/*******************************/


/***********************************/
/***    Variable CONFIGURATION   ***/
/***********************************/
volatile rawEncoderValue_t       enc_tick_cntr       = 0;
volatile rawRevEncoderValue_t    enc_revs_cntr       = 0;
volatile rawEncoderValue_t       enc_null_revs_cntr  = 0;
volatile bool                    enc_dir_state       = 0;

int32_t curr_time                = 0,
        prev_time                = 0,
        prev_prev_time           = 0,
        measured_time_width      = 0;


int32_t overflow_counter    = 0;
int32_t prev_overflow_cntr  = 0;

static bool         lld_encorder_Initialized        = false;
static bool         mRotating                       = false;

/* PartOfWheelRevPerMinute stands for part of revolution per minute
 * uses for velocity calculation   */
static float        velocityCalcTicksToRPM  = 0;

static int32_t      encoderTimerMaxOverflows    = 0;
/***********************************/



/**********************************/
/***    GPT UNIT CONFIGURATION  ***/
/**********************************/
static void gpt_overflow_cb ( GPTDriver *timeIntervalsDriver );
static GPTDriver                        *timeIntervalsDriver = &GPTD3;
/* Timer period = 50 ms */
#define TimerPeriod             50000
static const GPTConfig timeIntervalsCfg = {
                                             .frequency      =  1000000,// 1 MHz
                                             .callback       =  gpt_overflow_cb,
                                             .cr2            =  0,
                                             .dier           =  0U

};

/* Timer 3 overflow callback function */
static void gpt_overflow_cb(GPTDriver *gptd)
{
    (void)gptd;

    /* Increment overflow counter*/
    overflow_counter ++;

    if ( overflow_counter >= encoderTimerMaxOverflows )
    {
        mRotating = false; // not moved
    }
}


/***    Base channel processing     ***/
static void extcb_base(EXTDriver *extp, expchannel_t channel)
{
    (void)extp;
    (void)channel;

    /***    To define direction of encoder rotation  ***/
    if( enc_dir_state )   enc_tick_cntr    += 1;   // counterclockwise
    else                  enc_tick_cntr    -= 1;   // clockwise


    /***    Reset counter when it reaches the MAX value  ***/
    /***    Count encoder revolutions                    ***/
    if( enc_tick_cntr == (ENC_MAX_TICK_NUM - 1) )
    {
        enc_revs_cntr   += 1;
        enc_tick_cntr    = 0;
    }
    else if( enc_tick_cntr == (-ENC_MAX_TICK_NUM + 1) )
    {
        enc_revs_cntr   -= 1;
        enc_tick_cntr    = 0;
    }


    /* Velocity calculation */
    curr_time   = gptGetCounterX(timeIntervalsDriver);
    measured_time_width      = 0;

    if ( mRotating )
    {
        measured_time_width = curr_time + (overflow_counter-1) * TimerPeriod + (TimerPeriod - prev_time) ;

    }

    mRotating = true;

    prev_overflow_cntr = 0;
    overflow_counter = 0;


    prev_prev_time = prev_time;
    prev_time = curr_time;



}


/***    Secondary channel processing     ***/
static void extcb_dir(EXTDriver *extp, expchannel_t channel)
{
    (void)extp;
    (void)channel;

    if( palReadLine( ENCODER_CH_A_LINE ) == 0 )
        enc_dir_state    = 1;       // counterclockwise
    else
        enc_dir_state    = 0;       // clockwise

}

/***    NULL Point of encoder processing    ***/
static void extcb_null(EXTDriver *extp, expchannel_t channel)
{
    (void)extp;
    (void)channel;

    if( enc_dir_state == 0 )  enc_null_revs_cntr -= 1;
    else                      enc_null_revs_cntr += 1;


}

/********************************/
/*** Configuration structures ***/
/********************************/





/**
 * @brief   Initialize periphery connected to encoder
 * @note    Stable for repeated calls
 */
void lldEncoderInit( void )
{
    if ( lld_encorder_Initialized )
            return;

    /*EXT driver initialization*/
    commonExtDriverInit();

    /* Define channel config structure */
    EXTChannelConfig A_ch_conf, B_ch_conf, NULL_ch_conf;

    /* Fill in configuration for channel */

    A_ch_conf.mode     = EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOF; // EXT_CH_MODE_BOTH_EDGES
    A_ch_conf.cb       = extcb_base;

    B_ch_conf.mode     = EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOF;
    B_ch_conf.cb       = extcb_dir;

    NULL_ch_conf.mode  = EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOF;
    NULL_ch_conf.cb    = extcb_null;


    /* Set up EXT channel hardware pin mode as digital input  */
    palSetLineMode( ENCODER_CH_A_LINE, PAL_EVENT_MODE_RISING_EDGE ); //PAL_MODE_INPUT_PULLDOWN ??
    palSetLineMode( ENCODER_CH_B_LINE, PAL_EVENT_MODE_RISING_EDGE );
   // palSetLineMode( ENCODER_NULL_LINE, PAL_MODE_INPUT_PULLDOWN );

    /* Set channel (second arg) mode with filled configuration */
    extSetChannelMode( &EXTD1, ENCODER_CH_A_PAD, &A_ch_conf );
    extSetChannelMode( &EXTD1, ENCODER_CH_B_PAD, &B_ch_conf );
    //extSetChannelMode( &EXTD1, ENCODER_NULL_PAD, &NULL_ch_conf );


    /* Start working GPT driver in continuous (asynchronous) mode */
    gptStart( timeIntervalsDriver, &timeIntervalsCfg );
    gptStartContinuous( timeIntervalsDriver, TimerPeriod );

    /* Reset TMR overflow counter */
    overflow_counter = 0;

    /* Some calculations */
    encoderTimerMaxOverflows = encoderTimeoutMs /
               (TimerPeriod * 1000.0 /* to ms */ / timeIntervalsCfg.frequency);

    velocityCalcTicksToRPM  = 60.0 * timeIntervalsCfg.frequency / ENC_MAX_TICK_NUM;


    /* Set initialization flag */
    lld_encorder_Initialized = true;
}

/**
 *
 */
bool motorIsRotating( void )
{
    return mRotating;
}

/**
 *
 */
void resetEncoder ( void )
{
    enc_tick_cntr = 0;
}

/**
 * @brief   Get number of encoder ticks
 * @note    Max number of ticks is defined by MAX_TICK_NUM
 * @return  Encoder ticks number depends on direction of rotation
 *          [0; ENC_MAX_TICK_NUM]
 *          after ENC_MAX_TICK_NUM it resets
 */
rawEncoderValue_t lldGetEncoderRawTicks( void )
{
    if ( !lld_encorder_Initialized )
    {
        return -1;
    }
    return enc_tick_cntr;
}


/**
 * @brief   Get direction of encoder rotation
 * @return  clockwise           -> 0
 *          counterclockwise    -> 1
 */
bool lldGetEncoderDirection( void )
{
    if ( !lld_encorder_Initialized )
    {
        return -1;
    }
    return enc_dir_state;
}


/**
 * @brief   Get number of encoder revolutions [double]
 * @note    1 revolution = MAX_TICK_NUM ticks
 * @return  Encoder revolutions number depends on direction of rotation
 */
rawRevEncoderValue_t   lldGetEncoderRawRevs( void )
{
    if ( !lld_encorder_Initialized )
    {
        return -1;
    }
    return ( enc_revs_cntr + ( (float)enc_tick_cntr / (float)ENC_MAX_TICK_NUM ) );
}


/**
 * @brief   Get number of encoder revolutions [int]
 * @note    If you use absolute encoder!!!
 * @return  Encoder revolutions number depends on direction of rotation
 */
rawEncoderValue_t   lldGetAbsoluteEncoderRawRevs( void )
{
    if ( !lld_encorder_Initialized )
    {
        return -1;
    }
    return enc_null_revs_cntr;
}


/**
 * @ brief                            Gets motor rotation current velocity value
 *                                    [revolutions per minute (rpm)]
 * @ note                             Function use value ImpsPerRevQuantity  -
 *                                    Defined number of impulses per revolution
 *                                    depends on given sensor
 * @ return                           Current motor velocity value [rpm]
 */
mVelocity_t encoderGetVelocity ( void )
{
    mVelocity_t  velocity = 0;

    if ( !lld_encorder_Initialized )
    {
        return -1;
    }

    if ( !mRotating )
        return 0;

    /* Protection of division by zero.
     * measured_width = 0 if fronts counter < 2,
     * which means start and probably incorrect velocity calculation */
    if ( measured_time_width != 0)
    {
        /**
         * velocity = dx/dt, which means constant part of the revolution divided
         *                   by variable time width.
         *                   - dx is constant and equal 1/ENC_MAX_TICK_NUM.
         *                   - dt is equal time, mesured between two encoder base
         *                        channel fronts.
         *
         *  dt = timer counter / timer frequency [sec]
         *  ------------------------------------------
         *  velocity_rps = (1/ENC_MAX_TICK_NUM) / (timer counter / timer frequency)
         *  [rev per sec]
         *  ------------------------------------------
         *  Convert velocity units in [rev per minute]:
         *  velocity_rpm = velocity * 60
         *  ____________________________________________
         *  Thus:
         *  velocity_rpm = ( 60 * timer frequency / ENC_MAX_TICK_NUM ) / timer counter
         *  (numerator of this formula is constant and calculates ones. See function lldEncoderInit())
         */
        velocity = velocityCalcTicksToRPM / measured_time_width;
    }
    else
    {
        velocity = 0;
    }

    return velocity;
}

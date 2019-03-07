#include <lld_encoder.h>
#include <lld_encoder.h>

#define ENC_MAX_TICK_NUM        2000  // 1000 * 2 (BOTH_EDGES)

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

/***********************************/


/***    Base channel processing     ***/
static void extcb_base(EXTDriver *extp, expchannel_t channel)
{
    (void)extp;
    (void)channel;

    enc_tick_cntr    += 1;

#if 0
    /***    To define direction of encoder rotation  ***/
    if( enc_dir_state )   enc_tick_cntr    += 1;   // counterclockwise
    else                  enc_tick_cntr    -= 1;   // clockwise
#endif


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

    if ( palReadLine ( ENCODER_CH_A_LINE ) == 1 )
    {
        palSetLine( LINE_LED2 );
    }
    else
    {
        palClearLine( LINE_LED2 );
    }

}

static void extcb_dir(EXTDriver *extp, expchannel_t channel)
{
    (void)extp;
    (void)channel;

    if( palReadLine( ENCODER_CH_A_LINE ) == 0 )
        enc_dir_state    = 1;       // counterclockwise
    else
        enc_dir_state    = 0;       // clockwise

    //palClearLine( LINE_LED2 );

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



static bool         isInitialized       = false;

/**
 * @brief   Initialize periphery connected to encoder
 * @note    Stable for repeated calls
 */
void lldEncoderInit( void )
{
    //if ( isInitialized )
           // return;
    /*EXT driver initialization*/
    commonExtDriverInit();

    /* Define channel config structure */
    EXTChannelConfig A_ch_conf, B_ch_conf, NULL_ch_conf;

    /* Fill in configuration for channel */
    A_ch_conf.mode  = EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOF;
    A_ch_conf.cb    = extcb_base;
#if 1
    B_ch_conf.mode  = EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOF;
    B_ch_conf.cb    = extcb_dir;

    NULL_ch_conf.mode  = EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOF;
    NULL_ch_conf.cb    = extcb_null;
#endif


    /* Set up EXT channel hardware pin mode as digital input  */
    palSetLineMode( ENCODER_CH_A_LINE, PAL_MODE_INPUT_PULLDOWN ); //PAL_MODE_INPUT_PULLDOWN ??
    palSetLineMode( ENCODER_CH_B_LINE, PAL_MODE_INPUT_PULLDOWN );
    palSetLineMode( ENCODER_NULL_LINE, PAL_MODE_INPUT_PULLDOWN );

    /* Set channel (second arg) mode with filled configuration */
    extSetChannelMode( &EXTD1, ENCODER_CH_A_PAD, &A_ch_conf );
    extSetChannelMode( &EXTD1, ENCODER_CH_B_PAD, &B_ch_conf );
    extSetChannelMode( &EXTD1, ENCODER_NULL_PAD, &NULL_ch_conf );


    /* Set initialization flag */
    //isInitialized = true;
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
    return enc_tick_cntr;
}


/**
 * @brief   Get direction of encoder rotation
 * @return  clockwise           -> 0
 *          counterclockwise    -> 1
 */
bool lldGetEncoderDirection( void )
{
    return enc_dir_state;
}


/**
 * @brief   Get number of encoder revolutions [double]
 * @note    1 revolution = MAX_TICK_NUM ticks
 * @return  Encoder revolutions number depends on direction of rotation
 */
rawRevEncoderValue_t   lldGetEncoderRawRevs( void )
{
    return ( enc_revs_cntr + ( enc_tick_cntr / (float)ENC_MAX_TICK_NUM ) );
}


/**
 * @brief   Get number of encoder revolutions [int]
 * @note    If you use absolute encoder!!!
 * @return  Encoder revolutions number depends on direction of rotation
 */
rawEncoderValue_t   lldGetAbsoluteEncoderRawRevs( void )
{
    return enc_null_revs_cntr;
}


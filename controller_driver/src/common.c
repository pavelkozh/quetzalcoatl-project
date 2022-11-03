#include <common.h>
#include <string.h>

//Limit Switch

#define DRIVE_LIMIT_SWITCH_1_PIN            9
#define DRIVE_LIMIT_SWITCH_1_PIN_PORT       GPIOB
#define DRIVE_LIMIT_SWITCH_1_EXT_MODE_GPIO  EXT_MODE_GPIOB
#define DRIVE_LIMIT_SWITCH_2_PIN            14
#define DRIVE_LIMIT_SWITCH_2_PIN_PORT       GPIOG
#define DRIVE_LIMIT_SWITCH_2_EXT_MODE_GPIO  EXT_MODE_GPIOG


uint8_t uint8_map(uint8_t x, uint8_t in_min, uint8_t in_max, uint8_t out_min, uint8_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint32_t uint32_map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

double double_map(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
float float_map(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int8_t sign ( float var )
{
    bool result = 0;
    if ( var > 0 ) result = 1;
    if ( var < 0 ) result = -1;
    if ( var == 0 ) result = 0;
    return result;

}


/***********************/
/*** Common EXT unit ***/
/***********************/

static bool m_extDriverInitialized = false;

/**
 * @brief   Initialize EXT driver with empty config
 * @note    Safe to call any times, it checks state of previous call
 * @note    Must be called before EXT driver work
 */
void commonExtDriverInit ( void )
{
    if ( m_extDriverInitialized )
        return;

    static EXTConfig extcfg;
    for ( expchannel_t ch = 0; ch < EXT_MAX_CHANNELS; ch++ )
    {
        extcfg.channels[ch].mode  = EXT_CH_MODE_DISABLED;
        extcfg.channels[ch].cb    = NULL;
    }
    extStart( &EXTD1, &extcfg );



    /*EXT driver initialization for pedals*/

    EXTChannelConfig limit_switch_1_conf,limit_switch_2_conf;//1 - clutch, 2 - brake

    limit_switch_1_conf.mode  = EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | DRIVE_LIMIT_SWITCH_1_EXT_MODE_GPIO;
    limit_switch_1_conf.cb    = limit_switch_1_cb;

    limit_switch_2_conf.mode  = EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | DRIVE_LIMIT_SWITCH_2_EXT_MODE_GPIO;
    limit_switch_2_conf.cb    = limit_switch_2_cb;

    palSetLineMode( PAL_LINE(DRIVE_LIMIT_SWITCH_1_PIN_PORT,DRIVE_LIMIT_SWITCH_1_PIN), PAL_MODE_INPUT_PULLUP);
    palSetLineMode( PAL_LINE(DRIVE_LIMIT_SWITCH_2_PIN_PORT,DRIVE_LIMIT_SWITCH_2_PIN), PAL_MODE_INPUT_PULLUP);

    extSetChannelMode( &EXTD1, DRIVE_LIMIT_SWITCH_1_PIN, &limit_switch_1_conf );
    extSetChannelMode( &EXTD1, DRIVE_LIMIT_SWITCH_2_PIN, &limit_switch_2_conf );


    m_extDriverInitialized = true;
}


static void limit_switch_1_cb( EXTDriver *extp, expchannel_t channel ){
     (void) extp ;
     (void) channel ;

     palToggleLine(LINE_LED3);

}
static void limit_switch_2_cb( EXTDriver *extp, expchannel_t channel ){
    (void) extp ;
    (void) channel ;

    palToggleLine(LINE_LED3);
}









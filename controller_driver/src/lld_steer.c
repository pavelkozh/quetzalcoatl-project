#include <lld_steer.h>


/****************************************************************************/
/******************* STEER ENCODER SETUP ************************************/
/****************************************************************************/

void  steerEncReadValue( void );

#define ABS_ENC_SPI_DRIVER     &SPID2
#define ABS_ENC_CLK_LINE       PAL_LINE(GPIOB, 13)
#define ABS_ENC_CLK_MODE       PAL_MODE_ALTERNATE(5) //
#define ABS_ENC_MISO_LINE      PAL_LINE(GPIOC, 2)
#define ABS_ENC_MISO_MODE      PAL_MODE_ALTERNATE(5)
#define ABS_ENC_MOSI_LINE      PAL_LINE(GPIOB, 15)
#define ABS_ENC_MOSI_MODE      PAL_MODE_ALTERNATE(5)
#define ABS_ENC_CS_PORT        GPIOB
#define ABS_ENC_CS_PIN         12
#define ABS_ENC_CS_MODE        PAL_MODE_OUTPUT_PUSHPULL


#define ABS_ENC_CS_LINE    PAL_LINE(ABS_ENC_CS_PORT,ABS_ENC_CS_PIN)

#define SPI_BUFFERS_SIZE    1U

static uint16_t rxbuf[SPI_BUFFERS_SIZE];

static const SPIConfig spicfg = {
    .end_cb = NULL,
    .ssport = ABS_ENC_CS_PORT,
    .sspad = ABS_ENC_CS_PIN,
    .cr1 = SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0,// fPCLK/256
    .cr2 = SPI_CR2_DS //16-bit size mode
};

static bool if_steer_module_initialized = false;

void steerEncInit( void ) {
    if ( if_steer_module_initialized )
    {
        return;
    }

    /* Absolute encoder SPI init*/
    palSetLineMode( ABS_ENC_CLK_LINE , ABS_ENC_CLK_MODE );        // SPI SCK.
    palSetLineMode( ABS_ENC_MISO_LINE , ABS_ENC_MISO_MODE );    // MISO.
    palSetLineMode( ABS_ENC_MOSI_LINE , ABS_ENC_MOSI_MODE );    // MOSI.
    palSetLine( ABS_ENC_MOSI_LINE );
    palSetLineMode( ABS_ENC_CS_LINE , ABS_ENC_CS_MODE);        // CS0.
    palSetLine( ABS_ENC_CS_LINE );

    spiStart(ABS_ENC_SPI_DRIVER, &spicfg);

    /* Steer driver init */
    //
    //

    if_steer_module_initialized = true;
}


void  steerEncReadValue( void ){

	spiSelect(ABS_ENC_SPI_DRIVER); 
    spiReceive(ABS_ENC_SPI_DRIVER, 1, rxbuf);
    spiUnselect(ABS_ENC_SPI_DRIVER);
}      

float steerGetPosition( void ) {
    // palToggleLine(LINE_LED1);
	steerEncReadValue();
    // palToggleLine(LINE_LED3);
	int16_t ang = 0;
    ang = ( rxbuf[0] & 0b0111111111111000 ) >> 3; // main variable 
    if(ang > 2049) ang = ang - 4095;
    return (float)double_map(ang, -2048, 2049, -180, 180);
}

/****************************************************************************/
/******************* STEER DRIVER SETUP *************************************/
/****************************************************************************/

#define MOTOR_SPI_DRIVER         &SPID4
#define MOTOR_SPI_CLK_LINE       PAL_LINE(GPIOE, 2)
#define MOTOR_SPI_CLK_MODE       PAL_MODE_ALTERNATE(5)
#define MOTOR_SPI_MISO_LINE      PAL_LINE(GPIOE, 5)
#define MOTOR_SPI_MISO_MODE      PAL_MODE_ALTERNATE(5)
#define MOTOR_SPI_MOSI_LINE      PAL_LINE(GPIOE, 6)
#define MOTOR_SPI_MOSI_MODE      PAL_MODE_ALTERNATE(5)
#define MOTOR_SPI_CS_PORT        GPIOD
#define MOTOR_SPI_CS_PIN         3
#define MOTOR_SPI_CS_MODE        PAL_MODE_OUTPUT_PUSHPULL

#define MOTOR_SPI_CS_LINE        PAL_LINE(MOTOR_SPI_CS_PORT,MOTOR_SPI_CS_PIN)

#define MOTOR_SPI_BUFFERS_SIZE    1U


#define MOTOR_START_STOP_LINE    PAL_LINE(GPIOD, 4)
#define MOTOR_DIR_LINE           PAL_LINE(GPIOD, 5)



static uint16_t m_txbuf[MOTOR_SPI_BUFFERS_SIZE];
static uint16_t m_rxbuf[MOTOR_SPI_BUFFERS_SIZE];

static const SPIConfig m_spicfg = {
    .end_cb = NULL,
    .ssport = MOTOR_SPI_CS_PORT,
    .sspad = MOTOR_SPI_CS_PIN,
    .cr1 = SPI_CR1_BR_2 |SPI_CR1_BR_1 | SPI_CR1_BR_0, // fPCLK/64
    .cr2 = SPI_CR2_DS //16-bit size mode
};

static bool is_steer_motor_start = false; // 0-stop, 1-start
static thread_reference_t trp_steer_start_stop = NULL;
static bool is_steer_start_stop_change = false;
static THD_WORKING_AREA(steer_start_stop_wa, 512);
static THD_FUNCTION(steer_start_stop, arg) {
    (void)arg;
    while(1){

        if (!is_steer_start_stop_change){
            chSysLock();
            chThdSuspendS(&trp_steer_start_stop);
            chSysUnlock();
        }

        palClearLine(MOTOR_START_STOP_LINE);
        chThdSleepMilliseconds(200);
        palSetLine(MOTOR_START_STOP_LINE);
        is_steer_motor_start = !is_steer_motor_start;

        is_steer_start_stop_change = false;
        chSysLock();
        chThdSuspendS(&trp_steer_start_stop);
        chSysUnlock();

    }
}



static thread_reference_t trp_steer_dir_change = NULL;
static bool is_steer_dir_change = false;
static THD_WORKING_AREA(steer_dir_wa, 512);
static THD_FUNCTION(steer_dir, arg) {
    (void)arg;
    while(1){

        if (!is_steer_dir_change){
            chSysLock();
            chThdSuspendS(&trp_steer_dir_change);
            chSysUnlock();
        }

        palClearLine(MOTOR_DIR_LINE);
        chThdSleepMilliseconds(200);
        palSetLine(MOTOR_DIR_LINE);

        is_steer_dir_change = false;
        chSysLock();
        chThdSuspendS(&trp_steer_dir_change);
        chSysUnlock();

    }
}



void steerMotorInit( void ){
    // SPI for external DAC init
    palSetLineMode( MOTOR_SPI_CLK_LINE , MOTOR_SPI_CLK_MODE );   // SPI SCK.
    palSetLineMode( MOTOR_SPI_MISO_LINE , MOTOR_SPI_MISO_MODE ); // MISO.
    palSetLineMode( MOTOR_SPI_MOSI_LINE , MOTOR_SPI_MOSI_MODE ); // MOSI.
    palSetLine( MOTOR_SPI_MOSI_LINE );
    palSetLineMode( MOTOR_SPI_CS_LINE , MOTOR_SPI_CS_MODE);      // CS.
    palSetLine( MOTOR_SPI_CS_LINE );

    spiStart(MOTOR_SPI_DRIVER, &m_spicfg);

    // Start/Stop and Dir control pads int
    palSetLineMode ( MOTOR_START_STOP_LINE, PAL_MODE_OUTPUT_PUSHPULL );
    palSetLineMode ( MOTOR_DIR_LINE, PAL_MODE_OUTPUT_PUSHPULL );

    palSetLine(MOTOR_START_STOP_LINE);
    palSetLine(MOTOR_DIR_LINE);

    chThdCreateStatic(steer_start_stop_wa, sizeof(steer_start_stop_wa), NORMALPRIO + 2, steer_start_stop, NULL);
    chThdCreateStatic(steer_dir_wa, sizeof(steer_dir_wa), NORMALPRIO + 1, steer_dir, NULL);

}


void  steerMotorSetSpeed( float value ){
    if ( value < 0   ) value = 0;
    if ( value > 100 ) value = 100;
    uint16_t speed = (uint16_t) ( double_map(value, 0.0, 100.0, 0.0, 4095.0));
    m_txbuf[0] = ( 0b0111 << 12 ) | speed;

    spiSelect(MOTOR_SPI_DRIVER);
    spiSend(MOTOR_SPI_DRIVER, 1, m_txbuf);
    spiUnselect(MOTOR_SPI_DRIVER);
}


void steerMotorDirChange ( void ){
    if (is_steer_dir_change == false) {
        is_steer_dir_change = true;
        /* Wake up thread which starts or stops steer motor driver */
        chSysLock();
        chThdResume(&trp_steer_dir_change, MSG_OK);
        chSysUnlock();
      }

}


void steerMotorStartStopControl ( void ){
    if (is_steer_start_stop_change == false) {
        is_steer_start_stop_change = true;
        /* Wake up thread which starts or stops steer motor driver */
        chSysLock();
        chThdResume(&trp_steer_start_stop, MSG_OK);
        chSysUnlock();
      }

}

bool steerIsMotorEnable ( void ){
    return is_steer_motor_start;
}



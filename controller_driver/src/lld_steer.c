#include <lld_steer.h>

/****************************************************************************/
/******************* STEER ENCODER SETUP ************************************/
/****************************************************************************/

void steerEncReadValue(void);

#define ABS_ENC_SPI_DRIVER &SPID2
#define ABS_ENC_CLK_LINE PAL_LINE(GPIOB, 13)
#define ABS_ENC_CLK_MODE PAL_MODE_ALTERNATE(5) //
#define ABS_ENC_MISO_LINE PAL_LINE(GPIOC, 2)
#define ABS_ENC_MISO_MODE PAL_MODE_ALTERNATE(5)
#define ABS_ENC_MOSI_LINE PAL_LINE(GPIOB, 15)
#define ABS_ENC_MOSI_MODE PAL_MODE_ALTERNATE(5)
#define ABS_ENC_CS_PORT GPIOB
#define ABS_ENC_CS_PIN 12
#define ABS_ENC_CS_MODE PAL_MODE_OUTPUT_PUSHPULL

#define ABS_ENC_CS_LINE PAL_LINE(ABS_ENC_CS_PORT, ABS_ENC_CS_PIN)

#define SPI_BUFFERS_SIZE 1U

static uint16_t rxbuf[SPI_BUFFERS_SIZE];

static const SPIConfig spicfg = {
    .end_cb = NULL,
    .ssport = ABS_ENC_CS_PORT,
    .sspad = ABS_ENC_CS_PIN,
    .cr1 = SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0, // fPCLK/256
    .cr2 = SPI_CR2_DS                                  //16-bit size mode
};

static bool if_steer_module_initialized = false;

void steerEncInit(void)
{
    if (if_steer_module_initialized)
    {
        return;
    }

    /* Absolute encoder SPI init*/
    palSetLineMode(ABS_ENC_CLK_LINE, ABS_ENC_CLK_MODE);   // SPI SCK.
    palSetLineMode(ABS_ENC_MISO_LINE, ABS_ENC_MISO_MODE); // MISO.
    palSetLineMode(ABS_ENC_MOSI_LINE, ABS_ENC_MOSI_MODE); // MOSI.
    palSetLine(ABS_ENC_MOSI_LINE);
    palSetLineMode(ABS_ENC_CS_LINE, ABS_ENC_CS_MODE); // CS0.
    palSetLine(ABS_ENC_CS_LINE);

    spiStart(ABS_ENC_SPI_DRIVER, &spicfg);

    /* Steer driver init */
    //
    //

    if_steer_module_initialized = true;
}

void steerEncReadValue(void)
{
    spiAcquireBus(ABS_ENC_SPI_DRIVER);
    spiSelect(ABS_ENC_SPI_DRIVER);
    spiReceive(ABS_ENC_SPI_DRIVER, 1, rxbuf);
    spiUnselect(ABS_ENC_SPI_DRIVER);
    spiReleaseBus(ABS_ENC_SPI_DRIVER);
}

uint16_t steerGetPosition(void)//float
{
    // palToggleLine(LINE_LED1);
    steerEncReadValue();
    // palToggleLine(LINE_LED3);
    int16_t ang = 0;
    ang = (rxbuf[0] & 0b0111111111111000) >> 3; // main variable
    //return (float)double_map(ang, 0, 4095, 0, 360);
    return ang;
}


/****************************************************************************/
/******************* STEER DRIVER SETUP *************************************/
/****************************************************************************/

#define MOTOR_SPI_DRIVER &SPID4
#define MOTOR_SPI_CLK_LINE PAL_LINE(GPIOE, 2)
#define MOTOR_SPI_CLK_MODE PAL_MODE_ALTERNATE(5)
#define MOTOR_SPI_MISO_LINE PAL_LINE(GPIOE, 5)
#define MOTOR_SPI_MISO_MODE PAL_MODE_ALTERNATE(5)
#define MOTOR_SPI_MOSI_LINE PAL_LINE(GPIOE, 6)
#define MOTOR_SPI_MOSI_MODE PAL_MODE_ALTERNATE(5)
#define MOTOR_SPI_CS_PORT GPIOD
#define MOTOR_SPI_CS_PIN 3
#define MOTOR_SPI_CS_MODE PAL_MODE_OUTPUT_PUSHPULL

#define MOTOR_SPI_CS_LINE PAL_LINE(MOTOR_SPI_CS_PORT, MOTOR_SPI_CS_PIN)

#define MOTOR_SPI_BUFFERS_SIZE 1U

#define MOTOR_START_STOP_LINE PAL_LINE(GPIOD, 4)
#define MOTOR_DIR_LINE PAL_LINE(GPIOD, 5)

static uint16_t m_txbuf[MOTOR_SPI_BUFFERS_SIZE];
static uint16_t m_rxbuf[MOTOR_SPI_BUFFERS_SIZE];

static const SPIConfig m_spicfg = {
    .end_cb = NULL,
    .ssport = MOTOR_SPI_CS_PORT,
    .sspad = MOTOR_SPI_CS_PIN,
    .cr1 = SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0, // fPCLK/64
    .cr2 = SPI_CR2_DS                                  //16-bit size mode
};

#define DRIVER_SEND_PULSE_WIDTH_MS 100

static bool is_steer_motor_start = false; // 0-stop, 1-start
static bool is_steer_motor_dir = false;   // 0-stop, 1-start
static thread_reference_t trp_steer_start_stop = NULL;
static bool is_steer_start_stop_changed = true;

static THD_WORKING_AREA(steer_start_stop_wa, 512);
static THD_FUNCTION(steer_start_stop, arg)
{
    (void)arg;
    while (1)
    {
        chSysLock();
        chThdSuspendS(&trp_steer_start_stop);
        chSysUnlock();

        chThdSleepMilliseconds(DRIVER_SEND_PULSE_WIDTH_MS);
        palSetLine(MOTOR_START_STOP_LINE);
        is_steer_start_stop_changed = true;
    }
}

static thread_reference_t trp_steer_dir_change = NULL;
static bool is_steer_dir_changed = true;

static THD_WORKING_AREA(steer_dir_wa, 512);
static THD_FUNCTION(steer_dir, arg)
{
    (void)arg;
    while (1)
    {
        chSysLock();
        chThdSuspendS(&trp_steer_dir_change);
        chSysUnlock();

        chThdSleepMilliseconds(DRIVER_SEND_PULSE_WIDTH_MS);
        palSetLine(MOTOR_DIR_LINE);
        is_steer_dir_changed = true;
    }
}

void steerMotorInit(void)
{
    // SPI for external DAC init
    palSetLineMode(MOTOR_SPI_CLK_LINE, MOTOR_SPI_CLK_MODE);   // SPI SCK.
    palSetLineMode(MOTOR_SPI_MISO_LINE, MOTOR_SPI_MISO_MODE); // MISO.
    palSetLineMode(MOTOR_SPI_MOSI_LINE, MOTOR_SPI_MOSI_MODE); // MOSI.
    palSetLine(MOTOR_SPI_MOSI_LINE);
    palSetLineMode(MOTOR_SPI_CS_LINE, MOTOR_SPI_CS_MODE); // CS.
    palSetLine(MOTOR_SPI_CS_LINE);

    spiStart(MOTOR_SPI_DRIVER, &m_spicfg);

    // Start/Stop and Dir control pads int
    palSetLineMode(MOTOR_START_STOP_LINE, PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(MOTOR_DIR_LINE, PAL_MODE_OUTPUT_PUSHPULL);

    palSetLine(MOTOR_START_STOP_LINE);
    palSetLine(MOTOR_DIR_LINE);

    chThdCreateStatic(steer_start_stop_wa, sizeof(steer_start_stop_wa), NORMALPRIO + 2, steer_start_stop, NULL);
    chThdCreateStatic(steer_dir_wa, sizeof(steer_dir_wa), NORMALPRIO + 1, steer_dir, NULL);
}

void steerMotorSetSpeed(float value)
{
    if (value < 0)
        value = 0;
    if (value > 100)
        value = 100;
    uint16_t speed = (uint16_t)(double_map(value, 0.0, 100.0, 0.0, 4095.0));
    m_txbuf[0] = (0b0111 << 12) | speed;

    spiSelect(MOTOR_SPI_DRIVER);
    spiSend(MOTOR_SPI_DRIVER, 1, m_txbuf);
    spiUnselect(MOTOR_SPI_DRIVER);
}

void steerMotorDirChange(void)
{
    if (!is_steer_dir_changed)
        return;

    /* Wake up thread which starts or stops steer motor driver */
    palClearLine(MOTOR_DIR_LINE);
    is_steer_dir_changed = false;
    chThdResume(&trp_steer_dir_change, MSG_OK);
    is_steer_motor_dir = !is_steer_motor_dir;
}

void steerMotorStartStopControl(void)
{
    if (!is_steer_start_stop_changed)
        return;

    /* Wake up thread which starts or stops steer motor driver */
    palClearLine(MOTOR_START_STOP_LINE);
    is_steer_start_stop_changed = false;
    chThdResume(&trp_steer_start_stop, MSG_OK);
    is_steer_motor_start = !is_steer_motor_start;
}

bool steerIsMotorEnable(void)
{
    return is_steer_motor_start;
}

bool steerMotorEnableInvert(void)
{
    is_steer_motor_start = !is_steer_motor_start;
}

bool steerMotorGetDirection(void)
{
    return is_steer_motor_dir;
}

bool steerMotorDirectionInvert(void)
{
    is_steer_motor_dir = !is_steer_motor_dir;
}

/* Synchronization/Testing environment */

/* Percentage of speed to set for motor */
#define TEST_SPEED_PERC 5 //%
/* Time for rotation wait */
#define TEST_WAIT_TIME_MS 700
/* Minimal delta for movement recognition */
#define TEST_CHECK_ANGLE_DIFF 1.0
/* Limit for checking switches 0->360 / 360->0 */
#define TEST_LIMITS_SWITCH_MAX 200
/* Enable communication debug messages, comment #define to close */
#define TEST_DEBUG_ENABLED

int steerSyncTestDriver()
{
    // if (!is_initialized)
    // {
    // return EINVAL;
    // }

// clockwise = encoder ecreases
#define CLOCKWISE_DIR(diff) ((diff) < -TEST_CHECK_ANGLE_DIFF)
#define CCLOCKWISE_DIR(diff) ((diff) > TEST_CHECK_ANGLE_DIFF)
#define UPDATE_DIFF_NO_LIMITS(diff)                                 \
    (diff) = (diff) > TEST_LIMITS_SWITCH_MAX ? (diff)-360 : (diff); \
    (diff) = (diff) < -TEST_LIMITS_SWITCH_MAX ? (diff) + 360 : (diff);

#ifdef TEST_DEBUG_ENABLED
    comm_dbgprintf("--- Test step 1 ---\n");
#endif

    /* Step 1 - first check */
    bool expected_enbld = steerIsMotorEnable();
    bool expected_cwise_dir = steerMotorGetDirection();

    float initial_pos = steerGetPosition();

    steerMotorSetSpeed(TEST_SPEED_PERC);
    chThdSleepMilliseconds(TEST_WAIT_TIME_MS);
    steerMotorSetSpeed(0);

    float result_pos = steerGetPosition();
    float pos_diff = result_pos - initial_pos;

    UPDATE_DIFF_NO_LIMITS(pos_diff);

    if (abs(pos_diff) < TEST_CHECK_ANGLE_DIFF)
    {
#ifdef TEST_DEBUG_ENABLED
        comm_dbgprintf("No rotation %d / %d\n", expected_enbld, expected_cwise_dir);
#endif
        /* Result 1 - no rotation */
        if (expected_enbld)
        {
            /* Expected enabled - really not */
            steerMotorEnableInvert();
            comm_dbgprintf("Invert en\n");
        }
    }
    else
    {
#ifdef TEST_DEBUG_ENABLED
        comm_dbgprintf("Rotation %d / %d (%d)\n", expected_enbld, expected_cwise_dir, (int)pos_diff);
#endif
        /* Result 2 - rotate but check direction */
        if (!expected_enbld)
        {
            /* Expected disabled - really enabled */
            steerMotorEnableInvert();
#ifdef TEST_DEBUG_ENABLED
            comm_dbgprintf("Invert en\n");
#endif
        }

        if ((expected_cwise_dir && CCLOCKWISE_DIR(pos_diff)) ||
            (!expected_cwise_dir && CLOCKWISE_DIR(pos_diff)))
        {
            /* Expected and real differ! */
            steerMotorDirectionInvert();
#ifdef TEST_DEBUG_ENABLED
            comm_dbgprintf("Invert dir\n");
#endif
        }

        /* Disable it */
        steerMotorStartStopControl();

        /* Rotates, inverted direction if neede, everything is ok! */
        return EOK;
    }

#ifdef TEST_DEBUG_ENABLED
    comm_dbgprintf("--- Test step 2 ---\n");
#endif

    /* Step 2 - check direction / power */
    expected_cwise_dir = steerMotorGetDirection();
    if (!steerIsMotorEnable())
    {
        steerMotorStartStopControl();
        expected_enbld = steerIsMotorEnable();
    }

    initial_pos = steerGetPosition();

    steerMotorSetSpeed(TEST_SPEED_PERC);
    chThdSleepMilliseconds(TEST_WAIT_TIME_MS);
    steerMotorSetSpeed(0);

    result_pos = steerGetPosition();
    pos_diff = result_pos - initial_pos;

    UPDATE_DIFF_NO_LIMITS(pos_diff);

    if (abs(pos_diff) < TEST_CHECK_ANGLE_DIFF)
    {
#ifdef TEST_DEBUG_ENABLED
        comm_dbgprintf("No rotation %d / %d\n", expected_enbld, expected_cwise_dir);
#endif
        /* Result 3 - no rotation, but we inverted before! */
        if (expected_enbld)
        {
#ifdef TEST_DEBUG_ENABLED
            comm_dbgprintf("No power\n");
#endif
            /* Expected enabled - really not, no power then */
            return EFAULT;
        }
    }
    else
    {
#ifdef TEST_DEBUG_ENABLED
        comm_dbgprintf("Rotation %d / %d (%d)\n", expected_enbld, expected_cwise_dir, (int)pos_diff);
#endif
        /* Result 2 - rotate but check direction */
        if ((expected_cwise_dir && CCLOCKWISE_DIR(pos_diff)) ||
            (!expected_cwise_dir && CLOCKWISE_DIR(pos_diff)))
        {
            /* Expected and real differ! */
            steerMotorDirectionInvert();
#ifdef TEST_DEBUG_ENABLED
            comm_dbgprintf("Invert dir\n");
#endif
        }
    }

    /* Disable it */
    steerMotorStartStopControl();

    return EOK;
}

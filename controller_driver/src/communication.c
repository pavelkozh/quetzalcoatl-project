#include <hal.h>
#include <ch.h>
#include <stdio.h>
#include <chprintf.h>
#include <errno.h>
#include <communication.h>

#define EOK 0

static uint32_t vt_usb_check_period_MS = 10000;
/* USB configuration values ​​from usbcfg.c file */
extern const USBConfig usbcfg;
extern SerialUSBConfig serusbcfg;
extern SerialUSBDriver SDU1;

/* Flag to enable\disable debugging. */
static bool flag_debug = true;
static bool is_usb_mode = false;
static bool protocol_enabled = false;

static BaseSequentialStream *debug_stream = NULL; // The value read.
static BaseChannel *comm_chn = NULL;              // The value send.

static int retrieve_input_data(void);

#define INPUT_SYMB_CMD '&' // Starting byte for command.
#define INPUT_SYMB_CTL '#' // Starting byte to get new values.

/* Structure of velocity, angle and checksum values */
typedef struct
{
    uint8_t speed;
    uint8_t steer;
    uint8_t ck;
} input_cmd_t;

virtual_timer_t usb_check_vt;

communicationEventFun_t cpStructWithFunc;

communicationEventFun_t getDefaultCfg(void)
{
    communicationEventFun_t structFuncNull = {NULL, NULL, NULL, NULL};
    return structFuncNull;
}

/* Thread for read data */
static THD_WORKING_AREA(waConnection_action_n, 1024);
static THD_FUNCTION(Connection_action_n, arg)
{
    arg = arg;

    while (true)
    {
        if (is_usb_mode)
        {
            /* Each time we need to check as reading from non active leads to freeze */
            if (SDU1.config->usbp->state != USB_ACTIVE)
            {
                chThdSleepMilliseconds(20);
                continue;
            }
        }

        if (retrieve_input_data() == EOK)
        {
        }
    }
}

BaseChannel *comm_get_channel(void)
{
    return comm_chn;
}

/* Function to get data. */
static int retrieve_input_data(void)
{
    msg_t msg = chnGetTimeout(comm_chn, TIME_INFINITE);
    if (msg < 0)
    {
        if (is_usb_mode)
        {
            /* Q_RESET is returned in case of USB, when it is suddenly plugged out */
            if (msg == Q_RESET)
            {
            }
        }

        return ENODATA;
    }

    char start_byte = msg;
    if (start_byte == INPUT_SYMB_CTL)
    {
        input_cmd_t inp;

        msg = chnRead(comm_chn, (uint8_t *)&inp, sizeof(inp));

        if (msg != sizeof(inp))
        {

            return EIO;
        }

        /* Calculation and verification of checksum */
        uint8_t calc_ck = inp.speed + inp.steer * 2;
        if (calc_ck == inp.ck)
        {
            usb_is_alive();
            if (cpStructWithFunc.on_set)
                cpStructWithFunc.on_set(inp.speed, inp.steer);

            return EOK;
        }
    }

    else if (start_byte == INPUT_SYMB_CMD)
    {
        uint8_t rcv_buffer[3];

        msg = chnRead(comm_chn, rcv_buffer, sizeof(rcv_buffer));

        if (msg != sizeof(rcv_buffer))
        {
            return EIO;
        }

        /* Enable_debugging. */
        if (rcv_buffer[0] == 38 && rcv_buffer[1] == 79 && rcv_buffer[2] == 123)
        {
            usb_is_alive();
            flag_debug = 1;

            return EOK;
        }

        /* Disable_debugging. */
        if (rcv_buffer[0] == 31 && rcv_buffer[1] == 39 && rcv_buffer[2] == 115)
        {
            usb_is_alive();
            flag_debug = 0;

            return EOK;
        }

        /* Deactivate_connection. */
        if (rcv_buffer[0] == 67 && rcv_buffer[1] == 89 && rcv_buffer[2] == 23)
        {
            usb_is_alive();
            return EOK;
        }
        /* Stop_connection. */
        if (rcv_buffer[0] == 34 && rcv_buffer[1] == 63 && rcv_buffer[2] == 129)
        {
            usb_is_alive();
            return EOK;
        }
        /* On_start */
        if (rcv_buffer[0] == 25 && rcv_buffer[1] == 45 && rcv_buffer[2] == 65)
        {
            usb_is_alive();

            if (cpStructWithFunc.on_start)
                cpStructWithFunc.on_start();
            palSetLine(LINE_LED1);

            return EOK;
        }

        /* On_stop */
        if (rcv_buffer[0] == 13 && rcv_buffer[1] == 26 && rcv_buffer[2] == 39)
        {
            usb_is_alive();

            if (cpStructWithFunc.on_stop)
                cpStructWithFunc.on_stop();

            return EOK;
        }

        /* Steer straight */
        if (rcv_buffer[0] == 91 && rcv_buffer[1] == 8 && rcv_buffer[2] == 185)
        {
            usb_is_alive();

            if (cpStructWithFunc.on_steer_straight)
                cpStructWithFunc.on_steer_straight();

            return EOK;
        }
    }

    return ENODATA;
}

void usb_is_dead(void)
{
    if (cpStructWithFunc.on_interrupt_timer)
        cpStructWithFunc.on_interrupt_timer();
}

void usb_is_alive(void)
{
    chVTSet(&usb_check_vt, MS2ST(vt_usb_check_period_MS), usb_is_dead, NULL);
}

/* Initialization with a choice of USB or Serial. */
void comm_init(communicationEventFun_t *structWithFunc,
               uint32_t timeout_MS,
               bool usb_mode)
{
    is_usb_mode = usb_mode;

    if (timeout_MS > 0)
    {
        vt_usb_check_period_MS = timeout_MS;
    }

    if (structWithFunc)
    {
        cpStructWithFunc = *structWithFunc;
    }

    chVTObjectInit(&usb_check_vt);

    if (is_usb_mode)
    {
        sduObjectInit(&SDU1);
        sduStart(&SDU1, &serusbcfg);

        usbDisconnectBus(serusbcfg.usbp);
        usbStart(serusbcfg.usbp, &usbcfg);
        usbConnectBus(serusbcfg.usbp);

        debug_stream = (BaseSequentialStream *)&SDU1;
        comm_chn = (BaseChannel *)&SDU1;
    }
    else
    {
        static const SerialConfig sd_st_cfg = {
            .speed = 115200,
            .cr1 = 0,
            .cr2 = 0,
            .cr3 = 0};

        sdStart(&SD3, &sd_st_cfg);
        palSetPadMode(GPIOD, 8, PAL_MODE_ALTERNATE(7));
        palSetPadMode(GPIOD, 9, PAL_MODE_ALTERNATE(7));

        debug_stream = (BaseSequentialStream *)&SD3;
        comm_chn = (BaseChannel *)&SD3;
    }
}

void comm_start_protocol(void)
{
    protocol_enabled = true;
    chThdCreateStatic(waConnection_action_n,
                      sizeof(waConnection_action_n),
                      NORMALPRIO,
                      Connection_action_n,
                      NULL);
}

/* Sending a debug line without level */
void comm_dbgprintf(const char *format, ...)
{
    if (!debug_stream)
        return;

    if (!flag_debug)
        return;

    va_list ap;
    va_start(ap, format);
    chvprintf(debug_stream, format, ap);
    va_end(ap);
}

/* Sending a debug line with info. */
void comm_dbgprintf_info(const char *format, ...)
{
    if (!debug_stream)
        return;

    if (!flag_debug)
        return;

    char buffer[64];
    sprintf(buffer, "INF: %s", format);

    va_list ap;
    va_start(ap, format);
    chvprintf(debug_stream, buffer, ap);
    va_end(ap);
}

/* Sending a debug line with warning. */
void comm_dbgprintf_warning(const char *format, ...)
{
    if (!debug_stream)
        return;

    if (!flag_debug)
        return;

    char buffer[64];
    sprintf(buffer, "WARN: %s", format);

    va_list ap;
    va_start(ap, format);
    chvprintf(debug_stream, buffer, ap);
    va_end(ap);
}

/* Sending a debug line with error. */
void comm_dbgprintf_error(const char *format, ...)
{
    if (!debug_stream)
        return;

    if (!flag_debug)
        return;

    char buffer[64];
    sprintf(buffer, "ERR: %s", format);

    va_list ap;
    va_start(ap, format);
    chvprintf(debug_stream, buffer, ap);
    va_end(ap);
}

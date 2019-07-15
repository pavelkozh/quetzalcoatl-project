#include <hal.h>
#include <ch.h>
#include <stdio.h>
#include <chprintf.h>
#include <errno.h>
#include <communication.h>

#define EOK 0

static int16_t vt_usb_check_period_MS = 10000;
/* USB configuration values ​​from usbcfg.c file */
extern const USBConfig usbcfg;
extern SerialUSBConfig serusbcfg;
extern SerialUSBDriver SDU1;

/* Flag to enable\disable debugging. */
static bool flag_debug = 1;

static BaseSequentialStream *debug_stream = NULL; // The value read.
static BaseChannel *comm_chn = NULL; // The value send.

static int retrieve_input_data(void);

#define INPUT_SYMB_CMD  '&' // Starting byte for command.
#define INPUT_SYMB_CTL  '#' // Starting byte to get new values.

/* Structure of velocity, angle and checksum values */
typedef struct {
    uint8_t speed;
    uint8_t steer;
    uint8_t ck;
} input_cmd_t;

virtual_timer_t usb_check_vt;

communicationEventFun_t cpStructWithFunc;

communicationEventFun_t getDefaultCfg(void)
{
    communicationEventFun_t structFuncNull = {NULL, NULL, NULL, NULL};
    return  structFuncNull;
}


/* Thread for read data */
static THD_WORKING_AREA(waConnection_action_n, 1024);
static THD_FUNCTION(Connection_action_n, arg)
{
    arg = arg;
    
    while (true)
    {
#if (COMM_MODE == COMM_MODE_SERIAL_USB)
        /* Each time we need to check as reading from non active leads to freeze */
        if ( SDU1.config->usbp->state != USB_ACTIVE )
        {
            chThdSleepMilliseconds( 20 );
            continue;
        }
#endif

        if ( retrieve_input_data() == EOK )
        {
        }
    }
}

/* Function to get data. */
static int retrieve_input_data(void)
{
    msg_t msg = chnGetTimeout(comm_chn, TIME_INFINITE);
    if (msg < 0)
    {   
#if (COMM_MODE == COMM_MODE_SERIAL_USB)
        /* Q_RESET is returned in case of USB, when it is suddenly plugged out */
        if ( msg == Q_RESET )
        {
            
        }
#endif

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
            flag_debug = 1;

            return EOK;
        }

        /* Disable_debugging. */
        if (rcv_buffer[0] == 31 && rcv_buffer[1] == 39 && rcv_buffer[2] == 115)
        {
            flag_debug = 0;

            return EOK;
        }

        /* Deactivate_connection. */
        if (rcv_buffer[0] == 67 && rcv_buffer[1] == 89 && rcv_buffer[2] == 23)
        {
            return EOK;
        }
        /* Stop_connection. */
        if (rcv_buffer[0] == 34 && rcv_buffer[1] == 63 && rcv_buffer[2] == 129)
        {
            return EOK;
        }
        /* On_start */
        if (rcv_buffer[0] == 25 && rcv_buffer[1] == 45 && rcv_buffer[2] == 65)
        {
            if (cpStructWithFunc.on_start) 
                cpStructWithFunc.on_start();
            
            return EOK;
        }
        
        /* On_stop */
        if (rcv_buffer[0] == 13 && rcv_buffer[1] == 26 && rcv_buffer[2] == 39)
        {
            if (cpStructWithFunc.on_stop)
                cpStructWithFunc.on_stop();

            return EOK;
        }
    }

    return ENODATA;
}

void usb_is_dead(void)
{


    if(cpStructWithFunc.on_interrupt_timer)
        cpStructWithFunc.on_interrupt_timer();

    chVTSetI( &usb_check_vt, MS2ST( vt_usb_check_period_MS ), usb_is_dead, NULL );
}

void usb_is_alive(void)
    {
        chVTSet( &usb_check_vt, MS2ST( vt_usb_check_period_MS ), usb_is_dead, NULL );
    }

/* Initialization with a choice of USB or Serial. */
void comm_init(communicationEventFun_t structWithFunc, int16_t time_for_virtual_timer_MS)
{    
    cpStructWithFunc = structWithFunc;

    vt_usb_check_period_MS = time_for_virtual_timer_MS;

    chVTObjectInit(&usb_check_vt);
    usb_is_alive();

#if (COMM_MODE == COMM_MODE_SERIAL_USB)
    sduObjectInit(&SDU1);
    sduStart(&SDU1, &serusbcfg);

    usbDisconnectBus(serusbcfg.usbp);
    usbStart(serusbcfg.usbp, &usbcfg);
    usbConnectBus(serusbcfg.usbp);

    debug_stream = (BaseSequentialStream *)&SDU1;
    comm_chn = (BaseChannel *)&SDU1;
#elif (COMM_MODE == COMM_MODE_SERIAL_PROG)
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
#else
    chSysHalt("Communication module: mode not set");
#endif

    /* Thread creation. */
    chThdCreateStatic(waConnection_action_n, sizeof(waConnection_action_n), NORMALPRIO, Connection_action_n, NULL /* arg is NULL */);
}

/* Sending a debug line with info. */
void comm_dbgprintf_info(const char *format, ...)
{
    if (!debug_stream)
        return;

    // if (!flag_debug)
        // return;

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

// /* Return global variable speed from module. */
// comm_speed_t comm_get_speed(void)
// {
//     return speed_value;
// }

// /* Return global variable angle from module. */
// comm_steer_t comm_get_steer(void)
// {
//     return angle_value;
// }




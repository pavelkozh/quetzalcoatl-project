#include <hal.h>
#include <chprintf.h>
#include <errno.h>
#include <communication.h>
//#include <usbcfg.c>


#define EOK 0

#ifdef __cplusplus
extern "C" {
#endif

extern const USBConfig usbcfg;
extern SerialUSBConfig serusbcfg;
extern SerialUSBDriver SDU1;

#ifdef __cplusplus
}
#endif

static BaseSequentialStream *debug_stream = NULL;

void init_USB(void)
{
    sduObjectInit( &SDU1 );
    sduStart( &SDU1, &serusbcfg );


    usbDisconnectBus( serusbcfg.usbp );
    chThdSleepMilliseconds( 1500 );
    usbStart( serusbcfg.usbp, &usbcfg );
    usbConnectBus( serusbcfg.usbp );

    debug_stream = (BaseSequentialStream *)serusbcfg.usbp;

}

SerialDriver comm_dr = SDU1;

int SerialCommGetPkg(void)
{
    // if (p_pkg == NULL)
    //     return EINVAL;

    msg_t msg = sdGetTimeout(comm_dr, MS2ST(10));
    if (msg < 0)
    {
        return EIO;
    }

    char start_byte = msg;
    if (start_byte == '#')
    {
        int8_t rcv_buffer[2];
        int32_t rcv_bytes = sizeof(rcv_buffer);

        msg = sdReadTimeout(comm_dr, rcv_buffer, rcv_bytes, MS2ST(10));

        if (msg != rcv_bytes)
        {
            return EIO;
        }

        if(rcv_buffer[0] == 50 && rcv_buffer[1]==rcv_buffer[0] * 2)
        {
            palToggleLine(LINE_LED1);
        }
        
  
    }

    else if(start_byte == '$')
    {
        int8_t rcv_buffer[2];
        int32_t rcv_bytes = sizeof(rcv_buffer);

        msg = sdReadTimeout(comm_dr, rcv_buffer, rcv_bytes, MS2ST(10));

        if (msg != rcv_bytes)
        {
            return EIO;
        }

        if(rcv_buffer[0] == 70 && rcv_buffer[1]==rcv_buffer[0] * 2)
        {
            palToggleLine(LINE_LED2);
        }
        

    }

    else if(start_byte == '&')
    {
        int8_t rcv_buffer[3];
        int32_t rcv_bytes = sizeof(rcv_buffer);

        msg = sdReadTimeout(comm_dr, rcv_buffer, rcv_bytes, MS2ST(10));

        if (msg != rcv_bytes)
        {
            return EIO;
        }

        if(rcv_buffer[0] == 67 && rcv_buffer[1]== 89 && rcv_buffer[2] == 23)
        {
            palToggleLine(LINE_LED2);
        }
        

    }

    

    return EOK;
}




void dbgprintf( const char* format, ... )
{
    if ( !debug_stream )
        return;

    va_list ap;
    va_start(ap, format);
    chvprintf(debug_stream, format, ap);
    va_end(ap);
}




#ifndef _COMMUNICATION_
#define _COMMUNICATION_

#include <stdint.h>

typedef int8_t comm_speed_t;
typedef int8_t comm_steer_t;

typedef struct
{

    void (*on_set)(comm_speed_t speed, comm_steer_t angle);
    void (*on_start)(void);
    void (*on_stop)(void);
    void (*on_gear0)(void);
    void (*on_gear1)(void);
    void (*on_gear6)(void);
    void (*on_steer_straight)(void);
    void (*on_interrupt_timer)(void);

} communicationEventFun_t;

communicationEventFun_t getDefaultCfg(void);

void comm_init(communicationEventFun_t *structWithFunc,
               uint32_t timeout_MS,
               bool usb_mode);
void comm_start_protocol(void);

void comm_dbgprintf(const char *format, ...);
void comm_dbgprintf_error(const char *format, ...);
void comm_dbgprintf_warning(const char *format, ...);
void comm_dbgprintf_info(const char *format, ...);

/* Use carefully, as USB can freeze controller! */
BaseChannel *comm_get_channel(void);

void haltSystem(char *msg);

#endif //_COMMUNICATION_

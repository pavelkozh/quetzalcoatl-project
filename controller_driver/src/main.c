#include <common.h>
#include <tests.h>
#include <chprintf.h>


#include <lwipthread.h>
#include <lwip/netif.h>


int main(void)
{
    chSysInit();
    halInit();

    /* Init LWIP */
    lwipthread_opts_t opts;
    struct ip_addr ip, gateway, netmask;
    IP4_ADDR(&ip, 10, 100, 7, 4);
    IP4_ADDR(&gateway, 10, 100, 7, 3);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    uint8_t macaddr[6] = {0xC2, 0xAF, 0x51, 0x05, 0xCA, 0x46};

    opts.address = ip.addr;
    opts.gateway = gateway.addr;
    opts.netmask = netmask.addr;
    opts.macaddress = macaddr;

    // BaseChannel *dbg_chn = comm_get_channel();
    lwipInit(&opts);

#if (MAIN_PROGRAM_ROUTINE != PROGRAM_ROUTINE_MASTER) // need fixing

    testsRoutines();

#else

    while (1)
    {
        chThdSleepMilliseconds(1000);
    };
#endif
}

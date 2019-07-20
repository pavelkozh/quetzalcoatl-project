#include <tests.h>
#include <chprintf.h>

#include <steer_control.h>
#include <lld_steer.h>

static void showHelp(void)
{
    comm_dbgprintf("Test LLD steer control module\n");
    comm_dbgprintf("Commands:\n");
    comm_dbgprintf("  1 - Start/stop\n");
    comm_dbgprintf("  2 - Change dir\n");
    comm_dbgprintf("  3 - Perform driver sync\n");
    comm_dbgprintf("  0 - Get motor expected state\n");
    comm_dbgprintf("  q - Increase reference\n");
    comm_dbgprintf("  a - Decrease reference\n");
    comm_dbgprintf("  h - show help\n");
    comm_dbgprintf("  space - Reset reference\n");
}

void testSteerPositionControl(void)
{
    comm_init(NULL, 0, false);
    BaseChannel *dbg_chn = comm_get_channel();

#define REF_UPPER_LIMIT 180
#define REF_LOWER_LIMIT -180
    float ref_input = 0;
    float ref_delta = 10;
    int sync_result = 0;

    steerInit();

    showHelp();

    while (1)
    {
        msg_t msg = chnGetTimeout(dbg_chn, MS2ST(100));
        if (msg != MSG_TIMEOUT)
        {
            char input = (char)msg;

            switch (input)
            {
            case 'h':
                showHelp();
                break;

            case '1':
                steerControlStart();
                comm_dbgprintf("Control start\n");
                break;

            case '2':
                steerControlStop();
                comm_dbgprintf("Control stop\n");
                break;

            case '3':
                sync_result = steerSyncTestDriver();
                comm_dbgprintf("Sync done, result: %d\n", sync_result);
                break;

            case '0':
                comm_dbgprintf("Pwr: %s / Dir: %s\n",
                               steerIsMotorEnable() ? "enabled" : "disabled",
                               steerMotorGetDirection() ? "clockwise" : "counterclockwise");
                comm_dbgprintf("StRef: %d / PsRef: %d / CPos: %d / PErr: %d\n",
                               (int)steerDbgGetMotorCalcSpeedRef(),
                               (int)steerDbgGetMotorPosRef(),
                               (int)steerGetPos(),
                               (int)steerDbgGetMotorCalcPosErr());
                break;

            case 'q':
                ref_input += ref_delta;
                ref_input = ref_input > REF_UPPER_LIMIT ? REF_UPPER_LIMIT : ref_input;
                steerSetPosition(ref_input);
                comm_dbgprintf("Ref increase to %d\n", (int)ref_input);
                break;

            case 'a':
                ref_input -= ref_delta;
                ref_input = ref_input < REF_LOWER_LIMIT ? REF_LOWER_LIMIT : ref_input;
                steerSetPosition(ref_input);
                comm_dbgprintf("Ref decrease to %d\n", (int)ref_input);
                break;

            case ' ':
                ref_input = 0;
                steerSetPosition(ref_input);
                comm_dbgprintf("Reset reference\n");
                break;
            }
        }

#ifdef OLD_TEST
        palToggleLine(LINE_LED1);

        sdReadTimeout(&SD3, sd_buff, 9, TIME_IMMEDIATE);

        if (sd_buff[4] == 'm')
            steerSetPosition(((float)atoi(sd_buff)) / 10);
        if (sd_buff[0] == 's')
            steerControlStart();
        if (sd_buff[0] == 'd')
            steerControlStop();
        chprintf((BaseSequentialStream *)&SD3, "PosRef %.02f\t Encoder %d\t PosErr %.02f\t PidOut %.02f\t MotorEn%d\t \n\r", steerDbgGetMotorPosRef(), (uint16_t)(steerGetPosition()), steerDbgGetMotorCalcPosErr(), steerDbgGetMotorCalcSpeedRef(), steerDbgGetEnableFlag());

        for (int i = 0; i < 9; i++)
        {
            sd_buff[i] = '?';
        }
#endif
        chThdSleepMilliseconds(100);
    }
}

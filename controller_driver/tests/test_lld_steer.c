#include <tests.h>
#include <lld_steer.h>
#include <communication.h>

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};

static void showHelp(void)
{
    comm_dbgprintf("Test LLD steer module \n\r");
    comm_dbgprintf("Commands: \n\r");
    comm_dbgprintf("  0 - Get motor expected state \n\r");
    comm_dbgprintf("  1 - Start/stop \n\r");
    comm_dbgprintf("  2 - Change dir \n\r");
    comm_dbgprintf("  3 - Perform driver sync \n\r");
    comm_dbgprintf("  q - Increase speed \n\r");
    comm_dbgprintf("  a - Decrease speed \n\r");
    comm_dbgprintf("  h - show help \n\r");
    comm_dbgprintf("  space - Reset speed \n\r");

}

void testSteer(void)
{
    sdStart( &SD3, &sdcfg );
    steerEncInit();
    steerMotorInit();

    comm_init(NULL, 0, false);
    BaseChannel *dbg_chn = comm_get_channel();

    // uint8_t sd_buff[10];
    float perc_speed = 0;
    float speed_delta = 1;
    int sync_result;

    showHelp();
    uint8_t sd_buff[10];

    while (1)
    {
        sdReadTimeout( &SD3, sd_buff, 9, TIME_IMMEDIATE );
        msg_t msg = chnGetTimeout(dbg_chn, MS2ST(100));
        if (msg != MSG_TIMEOUT)
        {
            char input = (char)msg;

            switch (input)
            {
            case '1':
                steerMotorStartStopControl();
                comm_dbgprintf("Change start/stop\n\r");
                break;

            case '2':
                steerMotorDirChange();
                comm_dbgprintf("Change direction\n\r");
                break;

            case '3':
                sync_result = steerSyncTestDriver();
                comm_dbgprintf("Sync done, result: %d\n\r", sync_result);
                break;

            case '0':
                comm_dbgprintf("Pwr: %s / Dir: %s / Pos: %d \n\r",
                               steerIsMotorEnable() ? "enabled" : "disabled",
                               steerMotorGetDirection() ? "clockwise" : "counterclockwise",
                               (int)steerGetPosition());
                break;

            case 'h':
                showHelp();
                break;

            case 'q':
                perc_speed += speed_delta;
                perc_speed = perc_speed > 100 ? 100 : perc_speed;
                steerMotorSetSpeed(perc_speed);
                comm_dbgprintf("Speed increase to %d\n\r", (int)perc_speed);
                break;

            case 'a':
                perc_speed -= speed_delta;
                perc_speed = perc_speed < 0 ? 0 : perc_speed;
                steerMotorSetSpeed(perc_speed);
                comm_dbgprintf("Speed decrease to %d\n\r", (int)perc_speed);
                break;

            case ' ':
                perc_speed = 0;
                steerMotorSetSpeed(perc_speed);
                comm_dbgprintf("Reset speed\n\r");
                break;
            }
        }
#ifdef OLD_TEST
        if (sd_buff[4] == 'm')
            steerMotorSetSpeed((float)(atoi(sd_buff)));
        if (sd_buff[0] == 's')
            steerMotorStartStopControl();
        if (sd_buff[0] == 'd')
            steerMotorDirChange();
        // palToggleLine(LINE_LED2);
        steerMotorStartStopControl();

        chprintf((BaseSequentialStream *)&SD3, "Result %d\t \n\r", (uint16_t)(steerGetPosition() * 100));
        // palToggleLine(LINE_LED3);

        for (int i = 0; i < 9; i++)
        {
            sd_buff[i] = '?';
        }
#endif

        chThdSleepMilliseconds(100);
    }
}

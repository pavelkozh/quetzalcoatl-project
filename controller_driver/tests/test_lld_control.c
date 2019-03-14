#include <tests.h>
#include <lld_control.h>
#include <lld_sensor_module.h>
#include <chprintf.h>


static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};


//static bool right_dir = 0, left_dir = 0, drive_enable = 0, drive_start = 0;
static uint16_t Ain = 0, period_s = 0;
static float duty_cucle = 0.0;
static uint8_t rcv_data = 0, sd_buff[10];
int16_t *sd_value;
int32_t rcv_value = 0;
pwmcnt_t pwm_period_ticks = 0;



void TestPWMRouting (void)
{
  /* Serial driver configuration*/
  sdStart( &SD3, &sdcfg );
  palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
  palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX



  commonADC1UnitInit();
  lldControlInit();






  while (1)
  {

      motorRun();

      /* Read data from serial. Send for example 000100 or 065536.
       * First character used for sign,
       * but "atoi" cannot understand nothing but digits...  */
      sdReadTimeout( &SD3, sd_buff, 6, TIME_IMMEDIATE   );
      rcv_value = atoi(sd_buff);

      mSetSpeed ( rcv_value );


      chprintf( (BaseSequentialStream *)&SD3, "%d\n\r", rcv_value );
      chThdSleepMilliseconds( 100 );

  }
}

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
int16_t rcv_value = 0;



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
      //Ain = commonADC1UnitGetValue ( 1 );
      //duty_cucle = (float)Ain * 10000.0 / 4096.0;
      //period_s = (  Ain * 1000 / 4096 );
      //pwmChangePeriod(&PWMD1, 5000);

      motorRun();


      /* read data from serial */
      sdReadTimeout( &SD3, sd_buff, 4, TIME_IMMEDIATE   );
      rcv_value = atoi(sd_buff);

      chprintf( (BaseSequentialStream *)&SD3, "%d\n\r", rcv_value );

      //motorSetSpeed ( rcv_value );
      //setPwmPeriod ( rcv_value );
      chThdSleepMilliseconds( 100 );
  }
}

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
int8_t *sd_value;



void TestPWMRouting (void)
{
  /* Serial driver configuration*/
  sdStart( &SD3, &sdcfg );
  palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
  palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX


  commonADC1UnitInit();
  lldCcontrolInit();

  bool motor_run_enable = 0;



  while (1)
  {
      Ain = commonADC1UnitGetValue ( 1 );
      //duty_cucle = (float)Ain * 10000.0 / 4096.0;
      //period_s = (  Ain * 1000 / 4096 );
      //pwmChangePeriod(&PWMD1, 5000);

      motor_run_enable =  ifDriverEnable ();



#if 1
      if ( motor_run_enable )
      {
          pwmEnableChannel( &PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH (&PWMD1,5000) );
      }

      else
      {
          pwmEnableChannel( &PWMD1, 0, 0 );
      }

#endif

      //chprintf( (BaseSequentialStream *)&SD7, "ADC:  %d\t En %d\t DirR %d\t DirL %d\t Duty %d\n\r",
           //     Ain, drive_enable, right_dir, left_dir, *sd_value   );


      /* read data from serial */
      //rcv_data = sdGetTimeout( &SD7, TIME_IMMEDIATE );
      int test = sdReadTimeout( &SD3, sd_buff, 4, TIME_IMMEDIATE   );
      sd_value = &sd_buff[3];

      uint8_t *sign;
      chprintf( (BaseSequentialStream *)&SD3, "%d\n\r",
                     *sd_value   );





      chThdSleepMilliseconds( 10 );
  }
}

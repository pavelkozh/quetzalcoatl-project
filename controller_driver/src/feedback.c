#include <feedback.h>
#include <lld_px4flow.h>
static gazelParam *gaz;

static const I2CConfig i2cfg2 = {
     STM32_TIMINGR_PRESC(15U)  |
     STM32_TIMINGR_SCLDEL(9U) | STM32_TIMINGR_SDADEL(9U) |
     STM32_TIMINGR_SCLH(21U)   | STM32_TIMINGR_SCLL(24U),
    0,              // CR1
    0,              // CR2
#if I2C_TEST_CALLBACK
    &i2cCallbackFunc
#else
    NULL            // Receive callback
#endif
};

static int32_t px4flow_data[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static uint8_t px4flow_cnt = 0;
static int32_t px4flow_sum = 0;
static int16_t last_px4flow = 0;
static bool    i2c_err_flag = 0;
static uint8_t i2c_err_cnt = 0;
static int32_t counter=0;

static uint8_t i=0;
static uint8_t a=0;
static float sum=0;
static float gaz_buf[20]={0,};

void px4_filter(){
      if(update() == MSG_OK){
          //palToggleLine(LINE_LED2);

        i2c_err_flag = 0;
        i2c_err_cnt = 0;
        //if(ground_distance()>1000){
          //px4flow_sum -= px4flow_data[px4flow_cnt];

          //if( ((last_px4flow - flow_comp_m_x())<500)&&((last_px4flow - flow_comp_m_x())>-500)) {
            //px4flow_data[px4flow_cnt]= flow_comp_m_x();
            //last_px4flow = flow_comp_m_x();
          //}else{
            //px4flow_data[px4flow_cnt]= last_px4flow;
          //}

          //px4flow_sum += px4flow_data[px4flow_cnt];
          //px4flow_cnt = px4flow_cnt >= 19 ? 0 : px4flow_cnt+1;
        //}
      }
      else{
        i2c_err_cnt++;
        if(i2c_err_cnt > 30){
            palToggleLine(LINE_LED1);
            i2c_err_flag = 1;

            i2c_err_cnt=0;

            i2cStop(&I2CD2);
            i2cStart(&I2CD2, &i2cfg2);
//
        }

      }
      //gaz->Speed_px4flow = (px4flow_sum /20.0)*0.0036 ; //0.0036  0.0144
      //if(gaz_buf[i]<0){
          //gaz_buf[i]=-gaz_buf[i];
      //}
      gaz_buf[i]= (flow_comp_m_x())*0.0036 ;
      if(i<20){
      i++;
      }else{
          i=0;
      }
      sum=0;
      for(a=0;a<20;a=a+1){
          sum=sum+gaz_buf[a];
      }
      gaz->Speed_px4flow = sum/20; //0.0036  0.0144
      //chprintf( (BaseSequentialStream *)&SD3, "speed:%f cnt:%f\n\r",gaz->Speed_px4flow,(float)1.0);
      //
      //counter++;
};


/*
 * Receiver thread.
 */
static THD_WORKING_AREA(can_rx_wa, 256);
static THD_FUNCTION(can_rx, arg) {
    arg = arg;

    event_listener_t el1;

    chRegSetThreadName("receiver");
    chEvtRegister(&CAND1.rxfull_event, &el1, 0);

    while (1)
    {
      canUpdate();
      px4_filter();

      //gaz -> Speed += 1; 
      chThdSleepMilliseconds( 5 );
    }
    chEvtUnregister(&CAND1.rxfull_event, &el1);

  }

static bool if_feedback_module_initialized = 0;

void feedbackInit(void){

    if ( if_feedback_module_initialized )
    {
        return;
    }

    gaz = (gazelParam *)gazelGetStruct();
    can_init();
    px4flowInit();
    chThdCreateStatic(can_rx_wa, sizeof(can_rx_wa), NORMALPRIO, can_rx, NULL);//prio +15

    if_feedback_module_initialized = 1;

};

float gazelGetEngineSpeed (void){
  return gaz->EngineSpeed ;
};

int8_t gazelGetDriverIsDemandEnginePercentTorque(void){
  return gaz->DriverIsDemandEnginePercentTorque;
};

int8_t gazelGetActualEnginePercentTorque (void){
  return gaz->ActualEnginePercentTorque ;
};

double gazelGetSpeed(void){
    if(i2c_err_flag){

        palSetLine(LINE_LED1);
        //return gaz->Speed;

    }
    else
        if(gaz->Speed <= 6){
            if(gaz->Speed_px4flow < 0)
              return -gaz->Speed_px4flow;
            else
             return gaz->Speed_px4flow;
        }
        else{
          return gaz->Speed;
        }
};

double gazelGetSpeed_px4flow(void){
  return gaz->Speed_px4flow;
};

uint8_t gazelGetBrakeSwitch(void){
  return gaz->BrakeSwitch;
};

uint8_t gazelGetClutchSwitch(void){
  return gaz->ClutchSwitch;
};

float gazelGetAcceleratorPedalPosition(void){
  return gaz->AcceleratorPedalPosition;
};

int8_t gazelGetPercentLoadAtCurrentSpeed(void){
  return gaz->PercentLoadAtCurrentSpeed;
};

double gazelGetEngineFuelRate(void){
  return gaz->EngineFuelRate;
};

double gazelGetEngineInstantaneousFuelEconomy(void){
  return gaz->EngineInstantaneousFuelEconomy;
};

double gazelGetEngineThrottleValve(void){
  return gaz->EngineThrottleValve;
};

double gazelGetBatteryPotential(void){
  return gaz->BatteryPotential;
};

double gazelGetBrakePedalPosition(void){
  return gaz->BrakePedalPosition;
};

bool gazelGetI2cErrFlag( void ){
  return i2c_err_flag;
}

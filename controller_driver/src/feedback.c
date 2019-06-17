
#include <feedback.h>

static gazelParam *gaz;

static int32_t px4flow_data[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static uint8_t px4flow_cnt = 0;
static int32_t px4flow_sum = 0;
static int16_t last_px4flow = 0;

void px4_filter(){
      if(update() == MSG_OK){

        if(ground_distance()>1000){
          px4flow_sum -= px4flow_data[px4flow_cnt];

          if( ((last_px4flow - flow_comp_m_x())<500)&&((last_px4flow - flow_comp_m_x())>-500)) {
            px4flow_data[px4flow_cnt]= flow_comp_m_x();
            last_px4flow = flow_comp_m_x();
          }else{
            px4flow_data[px4flow_cnt]= last_px4flow;
          }

          px4flow_sum += px4flow_data[px4flow_cnt];
          px4flow_cnt = px4flow_cnt >= 19 ? 0 : px4flow_cnt+1;
        }
      }
      else{
          chprintf( (BaseSequentialStream *)&SD3, "I2C error \r\n");
      }
      gaz->Speed_px4flow = (px4flow_sum /20.0)*0.0036 ; //0.0036  0.0144
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
      chThdSleepMilliseconds( 3 );
    }
    chEvtUnregister(&CAND1.rxfull_event, &el1);

  }

void feedbackInit(void){
  gaz = (gazelParam *)GazleGetStruct();
  can_init();
  px4flowInit();
  chThdCreateStatic(can_rx_wa, sizeof(can_rx_wa), NORMALPRIO + 5, can_rx, NULL);
};

float gazleGetEngineSpeed (void){
  return gaz->EngineSpeed ;
};

int8_t gazleGetDriverIsDemandEnginePercentTorque(void){
  return gaz->DriverIsDemandEnginePercentTorque;
};

int8_t gazleGetActualEnginePercentTorque (void){
  return gaz->ActualEnginePercentTorque ;
};

double gazleGetSpeed(void){
  if(gaz->Speed > 6)
    return gaz->Speed;
  else
    return gaz->Speed_px4flow;
};

double gazleGetSpeed_px4flow(void){
  return gaz->Speed_px4flow;
};

uint8_t gazleGetBrakeSwitch(void){
  return gaz->BrakeSwitch;
};

uint8_t gazleGetClutchSwitch(void){
  return gaz->ClutchSwitch;
};

float gazleGetAcceleratorPedalPosition(void){
  return gaz->AcceleratorPedalPosition;
};

int8_t gazleGetPercentLoadAtCurrentSpeed(void){
  return gaz->PercentLoadAtCurrentSpeed;
};

double gazleGetEngineFuelRate(void){
  return gaz->EngineFuelRate;
};

double gazleGetEngineInstantaneousFuelEconomy(void){
  return gaz->EngineInstantaneousFuelEconomy;
};

double gazleGetEngineThrottleValve(void){
  return gaz->EngineThrottleValve;
};

double gazleGetBatteryPotential(void){
  return gaz->BatteryPotential;
};

double gazleGetBrakePedalPosition(void){
  return gaz->BrakePedalPosition;
};

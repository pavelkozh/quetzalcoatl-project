
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
      gaz -> Speed += 1; 
      chThdSleepMilliseconds( 200 );
    }
    chEvtUnregister(&CAND1.rxfull_event, &el1);

  }

void feedbackInit(){
  gaz = (gazelParam *)GazleGetStruct();
  can_init();
  px4flowInit();
  chThdCreateStatic(can_rx_wa, sizeof(can_rx_wa), NORMALPRIO + 5, can_rx, NULL);
};

float GazleGetEngineSpeed (){
  return gaz->EngineSpeed ;
};

int8_t GazleGetDriverIsDemandEnginePercentTorque(){
  return gaz->DriverIsDemandEnginePercentTorque;
};

int8_t GazleGetActualEnginePercentTorque (){
  return gaz->ActualEnginePercentTorque ;
};

double GazleGetSpeed(){
  if(gaz->Speed > 6)
    return gaz->Speed;
  else
    return gaz->Speed_px4flow;
};

double GazleGetSpeed_px4flow(){
  return gaz->Speed_px4flow;
};

uint8_t GazleGetBrakeSwitch(){
  return gaz->BrakeSwitch;
};

uint8_t GazleGetClutchSwitch(){
  return gaz->ClutchSwitch;
};

float GazleGetAcceleratorPedalPosition(){
  return gaz->AcceleratorPedalPosition;
};

int8_t GazleGetPercentLoadAtCurrentSpeed(){
  return gaz->PercentLoadAtCurrentSpeed;
};

double GazleGetEngineFuelRate(){
  return gaz->EngineFuelRate;
};

double GazleGetEngineInstantaneousFuelEconomy(){
  return gaz->EngineInstantaneousFuelEconomy;
};

double GazleGetEngineThrottleValve(){
  return gaz->EngineThrottleValve;
};

double GazleGetBatteryPotential(){
  return gaz->BatteryPotential;
};

double GazleGetBrakePedalPosition(){
  return gaz->BrakePedalPosition;
};
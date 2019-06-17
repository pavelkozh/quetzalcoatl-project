#include <lld_px4flow.h>


    struct i2c_frame frame;
    struct i2c_integral_frame iframe;
    static i2cflags_t errors = 0;
    static uint8_t rxbuf[25];
    static uint8_t txbuf = 0x0;
      msg_t status = MSG_OK;
    systime_t tmo = MS2ST(10);


#if 0
// Following numbers give 500kHz I2C clock - waveform significantly more low than high
static const I2CConfig i2cfg2 = {
     STM32_TIMINGR_PRESC(5U)  |
     STM32_TIMINGR_SCLDEL(3U) | STM32_TIMINGR_SDADEL(3U) |
     STM32_TIMINGR_SCLH(3U)   | STM32_TIMINGR_SCLL(9U),
    0,              // CR1
    0,              // CR2
#if I2C_TEST_CALLBACK
    &i2cCallbackFunc
#else
    NULL            // Receive callback
#endif
};
#else
// Following numbers trimmed to give 100kHz clock - waveform reasonably symmetrical
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
#endif


void px4flowInit(void){

  #if 1
  /** Start I2C bus 2 **/
  i2cStart(&I2CD2, &i2cfg2);
  palSetPadMode(GPIOB, 10, PAL_STM32_OTYPE_OPENDRAIN | PAL_MODE_ALTERNATE(4));//PAL_STM32_OTYPE_OPENDRAIN | 
  palSetPadMode(GPIOB, 11, PAL_STM32_OTYPE_OPENDRAIN | PAL_MODE_ALTERNATE(4));
  #endif
}


// Simple frame
uint16_t frame_count() {
  return frame.frame_count;
}

int16_t pixel_flow_x_sum() {
  return frame.pixel_flow_x_sum;
}

int16_t pixel_flow_y_sum() {
  return frame.pixel_flow_y_sum;
}

int16_t flow_comp_m_x() {
  return frame.flow_comp_m_x;
}

int16_t flow_comp_m_y() {
  return frame.flow_comp_m_y;
}

int16_t gyro_x_rate() {
  return frame.gyro_x_rate;
}

int16_t gyro_y_rate() {
  return frame.gyro_y_rate;
}

int16_t gyro_z_rate() {
  return frame.gyro_z_rate;
}

int16_t qual() {
  return frame.qual;
}

uint8_t sonar_timestamp() {
  return frame.sonar_timestamp;
}

int16_t ground_distance() {
  return frame.ground_distance;
}

// Integral frame
uint16_t frame_count_since_last_readout() {
  return iframe.frame_count_since_last_readout;
}

int16_t pixel_flow_x_integral() {
  return iframe.pixel_flow_x_integral;
}

int16_t pixel_flow_y_integral() {
  return iframe.pixel_flow_y_integral;
}

int16_t gyro_x_rate_integral() {
  return iframe.gyro_x_rate_integral;
}

int16_t gyro_y_rate_integral() {
  return iframe.gyro_y_rate_integral;
}

int16_t gyro_z_rate_integral() {
  return iframe.gyro_z_rate_integral;
}

uint32_t integration_timespan() {
  return iframe.integration_timespan;
}

uint32_t sonar_timestamp_integral() {
  return iframe.sonar_timestamp;
}
      
int16_t ground_distance_integral() {
  return iframe.ground_distance;
}

int16_t gyro_temperature() {
  return iframe.gyro_temperature;
}

uint8_t quality_integral() {
  return iframe.quality;
}

uint32_t read32(uint8_t a,uint8_t b,uint8_t c,uint8_t d) {
  return (uint32_t) read16(a,b) + (uint32_t) (read16(c,d) << 16);
}

uint16_t read16(uint8_t a,uint8_t b) {
  return rxbuf[a] + (uint16_t) (rxbuf[b] << 8);
}

uint8_t read8( uint8_t a) {
  return rxbuf[a];
}

bool update()
{
    txbuf = 0x0;
    i2cAcquireBus(&I2CD2);
    status = i2cMasterTransmitTimeout(&I2CD2, PX4FLOW_ADDRESS, &txbuf, 1, rxbuf, 22, tmo);
    i2cReleaseBus(&I2CD2);

    if(status != MSG_OK)
        return status;//i2cGetErrors(&I2CD2);

    // if (status != MSG_OK){
    //   errors = i2cGetErrors(&I2CD2);
    // }

    // read the data
      frame.frame_count       = read16(0,1);
      frame.pixel_flow_x_sum  = read16(2,3);
      frame.pixel_flow_y_sum  = read16(4,5);
      frame.flow_comp_m_x     = read16(6,7);
      frame.flow_comp_m_y     = read16(8,9);
      frame.qual              = read16(10,11);
      frame.gyro_x_rate       = read16(12,13);
      frame.gyro_y_rate       = read16(14,15);
      frame.gyro_z_rate       = read16(16,17);
      frame.gyro_range        = read8(18);
      frame.sonar_timestamp   = read8(19);
      frame.ground_distance   = read16(20,21);
    
    // if too many bytes are available, we drain in order to be synched
    // on next read
    
    return MSG_OK;
}

bool update_integral()
{
      txbuf = 0x16;
    //send 0x16 to PX4FLOW module and receive back 25 Bytes data 
    i2cAcquireBus(&I2CD2);
    status = i2cMasterTransmitTimeout(&I2CD2, PX4FLOW_ADDRESS, &txbuf, 1, rxbuf, 25, tmo);
    i2cReleaseBus(&I2CD2);
    
    if(status != MSG_OK)
        return status;

    // read the data
    iframe.frame_count_since_last_readout = read16(0,1);
    iframe.pixel_flow_x_integral  = read16(2,3);
    iframe.pixel_flow_y_integral  = read16(4,5);
    iframe.gyro_x_rate_integral   = read16(6,7);
    iframe.gyro_y_rate_integral   = read16(8,9);
    iframe.gyro_z_rate_integral   = read16(10,11);
    iframe.integration_timespan   = read32(12,13,14,15);
    iframe.sonar_timestamp        = read32(16,17,18,19);
    iframe.ground_distance        = read16(20,21);
    iframe.gyro_temperature       = read16(22,23);
    iframe.quality                = read8(24);


  return true;
}
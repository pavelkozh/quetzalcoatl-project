#include <lld_gyroscope.h>
#include <chprintf.h>

SPIDriver *spi4 = &SPID4;

static const SPIConfig spi4conf = {

    .end_cb = NULL,
    .ssport = GPIOD,
    .sspad = 3,
    .cr1 = SPI_CR1_BR_2 | SPI_CR1_BR_1,// 128 frequency prescaler
    .cr2 = SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0//8 bit data size
};

static uint8_t init_command[1] = {0x00};//init command
static uint16_t res=0;
static float angle = 0;

gyro_t angles = {0,0};

void gyroInit( void ){

  /** Start SPI bus**/

  palSetLineMode(PAL_LINE(GPIOD, 3), PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(PAL_LINE(GPIOE, 2), PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);
  palSetLineMode(PAL_LINE(GPIOE, 5), PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);
  palSetLineMode(PAL_LINE(GPIOE, 6), PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);

  spiStart(spi4, &spi4conf);

  spiSelect(spi4);
  spiSend(spi4,1,init_command);
  spiUnselect(spi4);

}

void spiReceiveGyroXY(void){

    uint8_t xy_txbuf[2] = {0x10, 0x11}; // X and Y output data register addresses
    uint8_t x_rxbuf[2] = {0,0};// result X
    uint8_t y_rxbuf[2] = {0,0};// result Y

    chThdSleepMicroseconds(150);
    //X receive
    spiSelect(spi4);
    spiSend(spi4,1,xy_txbuf);
    spiReceive(spi4,2,x_rxbuf);
    spiUnselect(spi4);
    angles.x_angle_deg = calculateGyroAngle(x_rxbuf);

    chThdSleepMicroseconds(15);

    //Y receive
    spiSelect(spi4);
    spiSend(spi4,1,xy_txbuf+1);
    spiReceive(spi4,2,y_rxbuf);
    spiUnselect(spi4);
    angles.y_angle_deg = calculateGyroAngle(y_rxbuf);

}

// calculate X/Y angle in degrees
// buf - x or y buffer
float calculateGyroAngle(uint8_t* buf){

    res = ((buf[0] << 8) | buf[1]) >> 5;
    angle =  ((float)res - OFFSET)/SENSITIVITY;
    angle = angle*180/3.14;//degrees

    return angle;
}

// get x angle
float getGyroX(void) {
  return angles.x_angle_deg;
}

// get y angle
float getGyroY(void) {
  return angles.y_angle_deg;
}


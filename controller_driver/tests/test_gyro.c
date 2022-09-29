#include <tests.h>
#include <chprintf.h>

#define OFFSET 1024
#define SENSITIVITY 819.0

static const SerialConfig sdcfg = {
  .speed = 115200,
  .cr1 = 0, .cr2 = 0, .cr3 = 0
};


void testGyro ( void )
{
    sdStart( &SD3, &sdcfg );
    palSetPadMode( GPIOD, 8, PAL_MODE_ALTERNATE(7) );   // TX
    palSetPadMode( GPIOD, 9, PAL_MODE_ALTERNATE(7) );   // RX

    palSetPadMode( GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 0, PAL_MODE_OUTPUT_PUSHPULL );    //Led
    palSetPadMode( GPIOB, 14, PAL_MODE_OUTPUT_PUSHPULL );   //Led

    palSetLineMode(PAL_LINE(GPIOD, 3), PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(PAL_LINE(GPIOE, 2), PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);
    palSetLineMode(PAL_LINE(GPIOE, 5), PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);
    palSetLineMode(PAL_LINE(GPIOE, 6), PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);

    SPIDriver *spi4 = &SPID4;

    static SPIConfig conf = {

        .end_cb = NULL,
        .ssport = GPIOD,
        .sspad = 3,
        .cr1 = SPI_CR1_BR_2 | SPI_CR1_BR_1,// 128 frequency prescaler
        .cr2 = SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0//8 bit data size
    };



    uint8_t xy_txbuf[2] = {0x10, 0x11}; // X and Y output data register addresses
    uint8_t x_rxbuf[2] = {0,0};// result X
    uint8_t y_rxbuf[2] = {0,0};// result Y
    uint16_t x_res = 0;
    uint16_t y_res = 0;

    float x_angle = 0.0;
    float y_angle = 0.0;
    uint8_t init_command[1] = {0x00};//init command
//    uint8_t temp_command[1] = {0x08};//temperature command
//    uint8_t temp_res[1] = {0};//temperature result

    spiStart(spi4, &conf);
    chprintf( (BaseSequentialStream *)&SD3, "Start\n\r");

    //init
    spiSelect(spi4);
    spiSend(spi4,1,init_command);
    spiUnselect(spi4);


    while(1) {

        chThdSleepMicroseconds(150);
        //X receive
        spiSelect(spi4);
        spiSend(spi4,1,xy_txbuf);
        spiReceive(spi4,2,x_rxbuf);
        spiUnselect(spi4);
        x_res = ((x_rxbuf[0] << 8) | x_rxbuf[1]) >> 5;
        x_angle =  ((float)x_res - OFFSET)/SENSITIVITY;//g
        x_angle = x_angle*180/3.14;//degrees
        chThdSleepMicroseconds(15);

        //Y receive
        spiSelect(spi4);
        spiSend(spi4,1,xy_txbuf+1);
        spiReceive(spi4,2,y_rxbuf);
        spiUnselect(spi4);
        y_res = ((y_rxbuf[0] << 8) | y_rxbuf[1]) >> 5;
        y_angle =  ((float)y_res - OFFSET)/SENSITIVITY;//g
        y_angle = y_angle*180/3.14;//degrees
        chThdSleepMicroseconds(15);

        palToggleLine(LINE_LED2);
        chprintf( (BaseSequentialStream *)&SD3, "X: %.2f Y: %.2f;\n\r",x_angle,y_angle);

        chThdSleepMilliseconds( 20 );

    }
}











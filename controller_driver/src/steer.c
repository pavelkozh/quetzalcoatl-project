#include <steer.h>

#define ABS_ENC_SPI_DRIVER     &SPID2
#define ABS_ENC_CLK_LINE       PAL_LINE(GPIOB, 13)
#define ABS_ENC_CLK_MODE       PAL_MODE_ALTERNATE(5) //
#define ABS_ENC_MISO_LINE      PAL_LINE(GPIOC, 2)
#define ABS_ENC_MISO_MODE      PAL_MODE_ALTERNATE(5)
#define ABS_ENC_MOSI_LINE      PAL_LINE(GPIOB, 15)
#define ABS_ENC_MOSI_MODE      PAL_MODE_ALTERNATE(5)
#define ABS_ENC_CS_PORT        GPIOB
#define ABS_ENC_CS_PIN         12
#define ABS_ENC_CS_MODE        PAL_MODE_OUTPUT_PUSHPULL


#define ABS_ENC_CS_LINE    PAL_LINE(ABS_ENC_CS_PORT,ABS_ENC_CS_PIN)

#define SPI_BUFFERS_SIZE    1U

static uint16_t rxbuf[SPI_BUFFERS_SIZE];

static const SPIConfig spicfg = {
    .end_cb = NULL,
    .ssport = ABS_ENC_CS_PORT,
    .sspad = ABS_ENC_CS_PIN,
    .cr1 = SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0,// fPCLK/8
    .cr2 = SPI_CR2_DS //16-bit size mode
};

void steerEncInit( void ) {
    palSetLineMode( ABS_ENC_CLK_LINE , ABS_ENC_CLK_MODE );        // SPI SCK.
    palSetLineMode( ABS_ENC_MISO_LINE , ABS_ENC_MISO_MODE );    // MISO.
    palSetLineMode( ABS_ENC_MOSI_LINE , ABS_ENC_MOSI_MODE );    // MOSI.
    palSetLine( ABS_ENC_MOSI_LINE );
    palSetLineMode( ABS_ENC_CS_LINE , ABS_ENC_CS_MODE);        // CS0.
    palSetLine( ABS_ENC_CS_LINE );

    //SPI init
    spiStart(ABS_ENC_SPI_DRIVER, &spicfg);
}


void  steerEncReadValue( void ){
	spiSelect(ABS_ENC_SPI_DRIVER);
    spiReceive(ABS_ENC_SPI_DRIVER, 1, rxbuf);
    spiUnselect(ABS_ENC_SPI_DRIVER);
}      

double steerGetPosition( void ) {
	steerEncReadValue();
	uint16_t ang = 0;
    ang = ( rxbuf[0] & 0b0111111111111000 ) >> 3; // main variable 
    return double_map(ang, 0, 4095, 0, 360);
}
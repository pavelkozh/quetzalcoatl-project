#include <lld_ext_dac.h>


#define EXTERNAL_DAC_SPI_DRIVER		&SPID1
#define EXTERNAL_DAC_CLK_LINE		PAL_LINE(GPIOA, 5)
#define EXTERNAL_DAC_CLK_MODE		PAL_MODE_ALTERNATE(5)
#define EXTERNAL_DAC_MISO_LINE		PAL_LINE(GPIOA, 6U)
#define EXTERNAL_DAC_MISO_MODE		PAL_MODE_ALTERNATE(5)
#define EXTERNAL_DAC_MOSI_LINE		PAL_LINE(GPIOB, 5U)// previous pin - PA7
#define EXTERNAL_DAC_MOSI_MODE		PAL_MODE_ALTERNATE(5)
#define EXTERNAL_DAC_CS_PORT		GPIOD
#define EXTERNAL_DAC_CS_PIN			14
#define EXTERNAL_DAC_CS_MODE		PAL_MODE_OUTPUT_PUSHPULL

#define EXTERNAL_DAC_CS_LINE		PAL_LINE(EXTERNAL_DAC_CS_PORT,EXTERNAL_DAC_CS_PIN)


/********SPI CONFIG**********/

#define SPI_BUFFERS_SIZE    1U

static uint16_t txbuf[SPI_BUFFERS_SIZE];
static uint16_t rxbuf[SPI_BUFFERS_SIZE];

static const SPIConfig spicfg = {
	.end_cb = NULL,
	.ssport = EXTERNAL_DAC_CS_PORT,
	.sspad = EXTERNAL_DAC_CS_PIN,
	.cr1 = SPI_CR1_BR | SPI_CR1_BR_0, // fPCLK/64
	.cr2 = SPI_CR2_DS //16-bit size mode
};

void extDacInit(){
	palSetLineMode( EXTERNAL_DAC_CLK_LINE , EXTERNAL_DAC_CLK_MODE ); 		// SPI SCK.
	palSetLineMode( EXTERNAL_DAC_MISO_LINE , EXTERNAL_DAC_MISO_MODE );    // MISO.
	palSetLineMode( EXTERNAL_DAC_MOSI_LINE , EXTERNAL_DAC_MOSI_MODE );    // MOSI.
	palSetLine( EXTERNAL_DAC_MOSI_LINE );
	palSetLineMode( EXTERNAL_DAC_CS_LINE , EXTERNAL_DAC_CS_MODE);        // CS0.
	palSetLine( EXTERNAL_DAC_CS_LINE );

	//SPI init
	spiStart(EXTERNAL_DAC_SPI_DRIVER, &spicfg);
}


void  extDacSetValue( uint8_t valueA, uint8_t valueB){
	//uint16_t val = (valueA<<8) | valueB;
	txbuf[0] =(0x01<<8) | valueA;
	txbuf[1] =(0x24<<8) | valueB;

	spiAcquireBus(EXTERNAL_DAC_SPI_DRIVER);

    spiSelect(EXTERNAL_DAC_SPI_DRIVER);
    spiSend(EXTERNAL_DAC_SPI_DRIVER, 1, txbuf);
    spiUnselect(EXTERNAL_DAC_SPI_DRIVER);
    spiSelect(EXTERNAL_DAC_SPI_DRIVER);
    spiSend(EXTERNAL_DAC_SPI_DRIVER, 1, txbuf+1);
    spiUnselect(EXTERNAL_DAC_SPI_DRIVER);

	spiReleaseBus(EXTERNAL_DAC_SPI_DRIVER);
}

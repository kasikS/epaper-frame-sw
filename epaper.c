#include <stdlib.h>
#include "epaper.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <string.h>
#include "serial.h"
#include "delay.h"


#define width EPD_WIDTH
#define height EPD_HEIGHT

//#define GPIOOff( x )  { WVS_PORT &= ~_BV(x); }
//#define GPIOOn( x )   { WVS_PORT |= _BV(x); }
//#define GPIORead( x ) ( !! (WVS_PIN & _BV(x) ) )

#define PORT 			GPIOB //which port??
#define WVS_CS_PIN		GPIO12 //which pin?
#define WVS_BUSY_PIN	GPIO11 // which pin?
#define WVS_CLK_PIN		GPIO10 // which pin?
#define WVS_MOSI_PIN	GPIO9 // which pin?
#define WVS_DC_PIN		GPIO8 // which pin?
#define WVS_RST_PIN		GPIO7 // which pin?


//#define WVS_PORT  PORTD
//#define WVS_PIN   PIND
//#define WVS_DDR   DDRD



static void EPD_5IN65F_BusyHigh( uint16_t timeout )// If BUSYN=0 then waiting
{
    while(!(gpio_get(PORT, WVS_BUSY_PIN)))
	{
		if( timeout-- == 0 ) break;
		delay_ms(1);
	}
}

static void EPD_5IN65F_BusyLow( uint16_t timeout )// If BUSYN=1 then waiting
{
    while(gpio_get(PORT, WVS_BUSY_PIN))
	{
		if( timeout-- == 0 ) break;
		delay_ms(1);
	}
}

static void wvs_Select(void) {
    gpio_clear(PORT, WVS_CS_PIN);
}

static void wvs_Unselect(void) {
    gpio_set(PORT, WVS_CS_PIN);
}

static void SpiTransfer( uint8_t data )
{
//	int bit;
//
//	GPIOOff( WVS_CS );  Delay();
//	GPIOOff( WVS_CLK ); Delay();
//
////	for( bit = 0x80; bit; bit>>= 1)
//	for( bit = 8; bit; bit-- )
//	{
//		if( data & 0x80 )
//		{
//			GPIOOn( WVS_DIN );
//		}
//		else
//		{
//			GPIOOff( WVS_DIN );
//		}
//		data<<=1;
//
//		Delay();
//		GPIOOn( WVS_CLK );
//		Delay();
//		GPIOOff( WVS_CLK );
//	}
//	GPIOOn( WVS_CS );

    spi_send(SPI2, data); 									//which SPI????
    while (SPI_SR(SPI2) & SPI_SR_BSY);


}

static void SendCommand( uint8_t command  )
{
   gpio_clear(PORT, WVS_DC_PIN);
//	GPIOOff( WVS_DC );
//	Delay();
    SpiTransfer( command );
}

static void SendData( uint8_t data  )
{
   gpio_set(PORT, WVS_DC_PIN);
//    GPIOOn( WVS_DC );
//	Delay();
    SpiTransfer( data );
}

void SetupEPaperForData(void)
{
	wvs_Select();

	SendCommand(0x61);//Set Resolution setting
    SendData(0x02);
    SendData(0x58);
    SendData(0x01);
    SendData(0xC0);
    SendCommand(0x10);

    wvs_Unselect();
}

void SendEPaperData(uint8_t * data, int len)
{
	wvs_Select();

	int i;
	for( i = 0; i < len; i++ )
		SendData( data[i] );

	wvs_Unselect();
}

void FlushAndDisplayEPaper(void)
{
	wvs_Select();

    SendCommand(0x04);//0x04
    EPD_5IN65F_BusyHigh(150);
    SendCommand(0x12);//0x12
    EPD_5IN65F_BusyHigh(15000);
    SendCommand(0x02);  //0x02
    EPD_5IN65F_BusyLow(150);
	delay_ms(20);

   wvs_Unselect();
}


void ClearEpaper(uint8_t color)
{

	SetupEPaperForData();
	uint8_t cv = (color<<4)|color;
    for(int i=0; i<width/2; i++) {
        for(int j=0; j<height; j++)
            SendData(cv);
    }
	FlushAndDisplayEPaper();
}


void EPD_5IN65F_Show7Block(void)
{
    unsigned long i,j,k;
    unsigned char const Color_seven[8] =
	{EPD_5IN65F_BLACK,EPD_5IN65F_BLUE,EPD_5IN65F_GREEN,EPD_5IN65F_ORANGE,
	EPD_5IN65F_RED,EPD_5IN65F_YELLOW,EPD_5IN65F_WHITE,7};
	SetupEPaperForData();

    for(i=0; i<224; i++) {
        for(k = 0 ; k < 4; k ++) {
            for(j = 0 ; j < 75; j ++) {
                SendData((Color_seven[k]<<4) |Color_seven[k]);
            }
        }
    }
    for(i=0; i<224; i++) {
        for(k = 4 ; k < 8; k ++) {
            for(j = 0 ; j < 75; j ++) {
                SendData((Color_seven[k]<<4) |Color_seven[k]);
            }
        }
    }
	FlushAndDisplayEPaper();
}




static void Clear(uint8_t color)
{
	SetupEPaperForData();
    for(int i=0; i<width/2; i++) {
        for(int j=0; j<height; j++)
            SendData((color<<4)|color);
    }
	FlushAndDisplayEPaper();
}


void SetupEPaperDisplay(void)
{
//#if 0
//	ConfigureGPIO( WVS_BUSY, INOUT_IN );
//	ConfigureGPIO( WVS_RESET, INOUT_OUT | DEFAULT_OFF );
//	ConfigureGPIO( WVS_DC, INOUT_OUT );
//	ConfigureGPIO( WVS_CS, INOUT_OUT | DEFAULT_ON );
//	ConfigureGPIO( WVS_CLK, INOUT_OUT | DEFAULT_OFF );
//	ConfigureGPIO( WVS_DIN, INOUT_OUT | DEFAULT_OFF );
//#endif

//#define WVS_PORT  PORTD
//#define WVS_PIN   PIND
//#define WVS_DDR   DDRD
//
//	GPIOOn( WVS_CS );
//	GPIOOff( WVS_CLK );
//	GPIOOff( WVS_DIN );
//	GPIOOff( WVS_RESET );
//
//	WVS_DDR &= ~_BV(WVS_BUSY);
//	WVS_DDR |= _BV(WVS_DIN);
//	WVS_DDR |= _BV(WVS_RESET);
//	WVS_DDR |= _BV(WVS_DC);
//	WVS_DDR |= _BV(WVS_CS);
//	WVS_DDR |= _BV(WVS_CLK);
//
//	WVS_PORT |= _BV(WVS_BUSY); //Pull-up.
//
//	//Reset for 1ms
//	_delay_ms( 10 );
//
//	GPIOOn( WVS_RESET );
//
//	_delay_ms( 10 );

//#if 0
//	_delay_ms(100);
//	while(1)
//	{
//		GPIOOn( WVS_CS );
//		GPIOOff( WVS_CS );
//	}
//#endif

////////// set proper pins, ports and mode for spi!!
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_SPI2);

    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO13);
    gpio_set_af(GPIOB, GPIO_AF5, GPIO13);
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO14);
    gpio_set_af(GPIOB, GPIO_AF5, GPIO14);
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO15);
    gpio_set_af(GPIOB, GPIO_AF5, GPIO15);
    gpio_mode_setup(PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, WVS_CS_PIN);

    spi_reset(SPI2);
    spi_init_master(SPI2, SPI_CR1_BAUDRATE_FPCLK_DIV_2, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);

    spi_set_full_duplex_mode(SPI2);
    spi_enable(SPI2);

/////configure reset, busy, dc!
    gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, WVS_RST_PIN);
    gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, WVS_DC_PIN);
    gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_NONE, WVS_BUSY_PIN);

	//Reset for 1ms
	gpio_clear(PORT, WVS_RST_PIN);
    delay_ms(10);
	gpio_set(PORT, WVS_RST_PIN);
	delay_ms(10);

///////////////
    serial_puts( "Epaper Leaving Reset\n" );
    EPD_5IN65F_BusyHigh( 20 );
    serial_puts( "Epaper Busy High Done\n" );


	wvs_Select();

    SendCommand(0x00);
    SendData(0xEF);
    SendData(0x08);
    SendCommand(0x01);
    SendData(0x37);
    SendData(0x00);
    SendData(0x23);
    SendData(0x23);
    SendCommand(0x03);
    SendData(0x00);
    SendCommand(0x06);
    SendData(0xC7);
    SendData(0xC7);
    SendData(0x1D);
    SendCommand(0x30);
    SendData(0x3C);
    SendCommand(0x40);
    SendData(0x00);
    SendCommand(0x50);
    SendData(0x37);
    SendCommand(0x60);
    SendData(0x22);
    SendCommand(0x61);
    SendData(0x02);
    SendData(0x58);
    SendData(0x01);
    SendData(0xC0);
    SendCommand(0xE3);
    SendData(0xAA);

    delay_ms(50);
    SendCommand(0x50);
    SendData(0x37);
    delay_ms(50);

	wvs_Unselect();
}

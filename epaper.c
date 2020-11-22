#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <stdlib.h>
#include <string.h>
#include "epaper.h"
#include "delay.h"

#define WVS_CS_PIN          GPIO14
#define WVS_CS_PORT         GPIOB
#define WVS_BUSY_PIN        GPIO12
#define WVS_BUSY_PORT       GPIOB
#define WVS_CLK_PIN         GPIO13
#define WVS_CLK_PORT        GPIOB
#define WVS_MOSI_PIN        GPIO15
#define WVS_MOSI_PORT       GPIOB

#define WVS_DC_PIN          GPIO8
#define WVS_DC_PORT         GPIOA
#define WVS_RST_PIN         GPIO9
#define WVS_RST_PORT        GPIOA


static void EPD_5IN65F_BusyHigh(uint16_t timeout)// If BUSYN=0 then waiting
{
    while(!(gpio_get(WVS_BUSY_PORT, WVS_BUSY_PIN)))
    {
        if( timeout-- == 0 )
            break;

        delay_ms(1);
    }
}

static void EPD_5IN65F_BusyLow(uint16_t timeout)// If BUSYN=1 then waiting
{
    while(gpio_get(WVS_BUSY_PORT, WVS_BUSY_PIN))
    {
        if( timeout-- == 0 )
            break;

        delay_ms(1);
    }
}

static inline void wvs_Select(void)
{
    gpio_clear(WVS_CS_PORT, WVS_CS_PIN);
}

static inline void wvs_Unselect(void)
{
    gpio_set(WVS_CS_PORT, WVS_CS_PIN);
}

static void SpiTransfer( uint8_t data )
{
    wvs_Select();
    spi_send(SPI2, data);
    while (SPI_SR(SPI2) & SPI_SR_BSY);
    wvs_Unselect();
}

static void SendCommand(uint8_t command)
{
    gpio_clear(WVS_DC_PORT, WVS_DC_PIN);
    SpiTransfer(command);
}

static void SendData(uint8_t data)
{
    gpio_set(WVS_DC_PORT, WVS_DC_PIN);
    SpiTransfer( data );
}

void SetupEPaperForData(void)
{
    SendCommand(0x61);//Set Resolution setting
    SendData(0x02);
    SendData(0x58);
    SendData(0x01);
    SendData(0xC0);
    SendCommand(0x10);
}

void SendEPaperData(const uint8_t* data, int len)
{
    for(int i = 0; i < len; i++ )
        SendData( data[i] );
}

void FlushAndDisplayEPaper(void)
{
    SendCommand(0x04);//0x04
    EPD_5IN65F_BusyHigh(150);
    SendCommand(0x12);//0x12
    EPD_5IN65F_BusyHigh(15000);
    SendCommand(0x02);  //0x02
    EPD_5IN65F_BusyLow(150);
    delay_ms(20);
}

void ClearEpaper(uint8_t color)
{
    const uint8_t cv = (color << 4) | color;
    SetupEPaperForData();

    for(int i = 0; i < EPD_WIDTH / 2; i++) {
        for(int j = 0; j < EPD_HEIGHT; j++)
        {
            SendData(cv);
        }
    }

    FlushAndDisplayEPaper();
}

void SetupEPaperDisplay(void)
{
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_SPI2);

    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO13);
    gpio_set_af(GPIOB, GPIO_AF0, GPIO13);
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO14);
    gpio_set_af(GPIOB, GPIO_AF0, GPIO14);
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO15);
    gpio_set_af(GPIOB, GPIO_AF0, GPIO15);
    gpio_mode_setup(WVS_CS_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, WVS_CS_PIN);
    gpio_set(WVS_CS_PORT, WVS_CS_PIN);

    spi_reset(SPI2);
    spi_init_master(SPI2, SPI_CR1_BAUDRATE_FPCLK_DIV_2, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);

    spi_set_full_duplex_mode(SPI2);
    spi_enable(SPI2);

    gpio_mode_setup(WVS_RST_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, WVS_RST_PIN);
    gpio_mode_setup(WVS_DC_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, WVS_DC_PIN);
    gpio_mode_setup(WVS_BUSY_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, WVS_BUSY_PIN);

    //Reset for 1ms
    gpio_clear(WVS_RST_PORT, WVS_RST_PIN);
    delay_ms(10);
    gpio_set(WVS_RST_PORT, WVS_RST_PIN);
    delay_ms(10);

    EPD_5IN65F_BusyHigh(20);

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
}

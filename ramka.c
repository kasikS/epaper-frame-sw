/*
 * Copyright (C) 2019 Maciej Suminski <orson@orson.net.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/cm3/scb.h>
#include <libopencmsis/core_cm3.h>

#include "serial.h"
#include "delay.h"
#include "sdcard.h"
#include "rtc.h"
#include "power.h"
#include "epaper.h"

/*#define DEBUG*/

#ifdef DEBUG
#define dbg_s(x) serial_puts(x)
#define dbg_c(x) serial_putc(x)
#else
#define dbg_s(x)
#define dbg_c(x)
#endif


#define SECTOR_SIZE         512
#define SECTORS_PER_IMAGE   263
static uint8_t sd_buf[SECTOR_SIZE];

unsigned int image_index = 0;

void rtc_isr(void)
{
    RTC_ISR &= ~(RTC_ISR_WUTF);
    exti_reset_request(EXTI20);
    dbg_c(image_index + '0');
}

static void setup_clocks(void)
{
    struct rcc_clock_scale a =
    { /* 4MHz HSI raw */
        .hpre = RCC_CFGR_HPRE_DIV4,
        .ppre1 = RCC_CFGR_PPRE1_NODIV,
        .ppre2 = RCC_CFGR_PPRE2_NODIV,
        .voltage_scale = PWR_SCALE1,
        .flash_waitstates = 0,
        .ahb_frequency  = 4000000,
        .apb1_frequency = 4000000,
        .apb2_frequency = 4000000,
    };
       rcc_clock_setup_pll(&a);
       /* Lots of things on all ports... */
       rcc_periph_clock_enable(RCC_GPIOA);
       rcc_periph_clock_enable(RCC_GPIOB);
       /* Enable clocks for USART2. */
       rcc_periph_clock_enable(RCC_USART2);
       /* And timers. */
       rcc_periph_clock_enable(RCC_TIM6);
       rcc_periph_clock_enable(RCC_TIM7);
       /* And syscfg for exti port mapping */
       rcc_periph_clock_enable(RCC_SYSCFG);
}


// Return codes for display_image()
#define DISPLAY_OK   0
#define SD_FAILURE   1
#define END_OF_DATA  2

static int display_image(unsigned int index)
{
    bool correct = false;

    power_sd(false);
    power_epaper(false);
    delay_ms(25);

    power_epaper(true);
    power_sd(true);
    delay_ms(25);

    if (SDCARD_Init()) {
        dbg_s("buuu:(");
        SDCARD_Deinit();
        power_sd(false);
        power_epaper(false);
        return SD_FAILURE;
    } else {
        dbg_s("sd ok");
    }

    SetupEPaperDisplay();
    SetupEPaperForData();
    SDCARD_ReadBegin(index * SECTORS_PER_IMAGE);

    // read image from SD and transfer it to epaper
    for(int i = 0; i < SECTORS_PER_IMAGE; ++i) {
        SDCARD_ReadData(sd_buf);

        // check if this is an empty image, indicating the end of data
        if (!correct) {
            for (int j = 0; j < SECTOR_SIZE; ++j) {
                if (sd_buf[j] != 0xff) {
                    correct = true;
                    break;
                }
            }
        }

        SendEPaperData(sd_buf, sizeof(sd_buf));
    }

    SDCARD_ReadEnd();
    power_sd(false);
    SDCARD_Deinit();

    if (correct)
        FlushAndDisplayEPaper();

    power_epaper(false);
    delay_ms(25);
    StopEPaperDisplay();

    return correct ? DISPLAY_OK : END_OF_DATA;
}

// TODO to use standby mode, the last image index needs to be stored in
// a backup register

int main(void)
{
    /*setup_clocks();*/     // TODO broken, do not run!

    power_init();
    delay_init();
    // leave 3 seconds to start flashing before entering deep sleep mode
    delay_ms(3000);

    #ifdef DEBUG
    serial_init(115200);
    dbg_s("siema");
    #endif

    power_init();
    rtc_init();
    /*rtc_set_wakeup(24 * 60 * 60);  // 24h       // TODO*/
    rtc_set_wakeup(30);

    while (1)
    {
        int result = display_image(image_index);

        if (result == DISPLAY_OK) {
            ++image_index;

        } else if (result == END_OF_DATA) {
            image_index = 0;
            display_image(image_index);

        } else if (result == SD_FAILURE) {
            // TODO
        }

        /*delay_ms(10000);*/

        PWR_CR |= PWR_CR_LPSDSR;    // voltage regulator in low power mode
        /*pwr_set_stop_mode();*/
        pwr_set_standby_mode();
        __WFI();
    }
}

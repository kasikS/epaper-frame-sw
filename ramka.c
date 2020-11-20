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

#include "serial.h"
#include "delay.h"
#include "sdcard.h"
#include "rtc.h"
#include "power.h"
#include "epaper.h"

uint8_t sd_buf[512];

static inline __attribute__((always_inline)) void __WFI(void)
{
	__asm volatile ("wfi");
}

void rtc_wkup_isr(void)
{
    RTC_ISR &= ~(RTC_ISR_WUTF);
    exti_reset_request(EXTI20);
    serial_puts("RTC!");
}

static void display_image(unsigned int index)
{
    power_sd(false);
    power_epaper(false);
    delay_ms(50);

    power_epaper(true);
    power_sd(true);
    delay_ms(200);

    SetupEPaperDisplay();

    if (SDCARD_Init()) {
        serial_puts("buuu:(");
        power_sd(false);
        power_epaper(false);
        return;
    } else {
        serial_puts("sd ok");
    }

    SetupEPaperForData();
    SDCARD_ReadBegin(index * 263);

    // read image from SD and transfer it to epaper
    for(int i = 0; i < 263; ++i) {
        SDCARD_ReadData(sd_buf);
        SendEPaperData(sd_buf, sizeof(sd_buf));
    }
    SDCARD_ReadEnd();

    FlushAndDisplayEPaper();

    delay_ms(100);
    power_sd(false);
    power_epaper(false);
}

int main(void)
{
    /*setup_clocks();*/
    char int_buf[6];
    int int_len;

    serial_init(115200);
    delay_init();
    rtc_init();
    power_init();
    if (SDCARD_Init() == 0)
    {
        serial_puts("jeeej!");
        memset(sd_buf, 0, 512);
        SDCARD_ReadBegin(0);
        SDCARD_ReadData(sd_buf);
        SDCARD_ReadEnd();

        for (int i = 0; i < 32; ++i)
        {
            int_len = toa(sd_buf[i], int_buf);
            serial_write(int_buf, int_len);
            serial_puts("\r\n");
        }
    }
    else
    {
        serial_puts("buuu:(");
    }

    rtc_set_wakeup(86400);  // 86400s == 24h


    while (1)
    {
        pwr_set_standby_mode();
        __WFI();

    }
}

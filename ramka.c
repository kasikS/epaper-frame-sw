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

static void setup_clocks(void)
{
    /* 4MHz MSI raw range 2*/
    struct rcc_clock_scale myclock_config = {
        .hpre = RCC_CFGR_HPRE_SYSCLK_NODIV,
        .ppre1 = RCC_CFGR_PPRE1_HCLK_NODIV,
        .ppre2 = RCC_CFGR_PPRE2_HCLK_NODIV,
        .voltage_scale = PWR_SCALE2,
        .flash_waitstates = FLASH_ACR_LATENCY_0WS,
        .apb1_frequency = 4194000,
        .apb2_frequency = 4194000,
        .msi_range = RCC_ICSCR_MSIRANGE_4MHZ,
    };

    rcc_clock_setup_msi(&myclock_config);
}


int main(void)
{
    /*setup_clocks();*/
    char int_buf[6];
    int int_len;

    serial_init(115200);
    delay_init();
    rtc_init();
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

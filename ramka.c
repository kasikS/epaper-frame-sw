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

#define SECTOR_SIZE         512
#define SECTORS_PER_IMAGE   263
static uint8_t sd_buf[SECTOR_SIZE];

static inline __attribute__((always_inline)) void __WFI(void)
{
    __asm volatile ("wfi");
}

void rtc_isr(void)
{
    RTC_ISR &= ~(RTC_ISR_WUTF);
    exti_reset_request(EXTI20);
    serial_puts("RTC!");
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
    delay_ms(50);

    power_epaper(true);
    power_sd(true);
    delay_ms(200);

    SetupEPaperDisplay();

    if (SDCARD_Init()) {
        serial_puts("buuu:(");
        power_sd(false);
        power_epaper(false);
        return SD_FAILURE;
    } else {
        serial_puts("sd ok");
    }

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

    if (correct)
        FlushAndDisplayEPaper();

    delay_ms(100);
    power_sd(false);
    power_epaper(false);

    return correct ? DISPLAY_OK : END_OF_DATA;
}

// TODO to use standby mode, the last image index needs to be stored in
// a backup register

int main(void)
{
    unsigned int image_index = 0;
    /*setup_clocks();*/

    serial_init(115200);
    delay_init();
    power_init();
    /*rtc_init();*/

    serial_puts("siema");
    /*rtc_set_wakeup(86400);  // 86400s == 24h*/

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

        delay_ms(10000);
        serial_putc('a');

        /*pwr_set_standby_mode();*/
        /*__WFI();*/
    }
}

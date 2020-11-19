#include "power.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#define SD_PWR_PORT         GPIOB
#define SD_PWR_PIN          GPIO1
#define EPAPER_PWR_PORT     GPIOA
#define EPAPER_PWR_PIN      GPIO10
#define ESP_PWR_PORT        GPIOC
#define ESP_PWR_PIN         GPIO13

void power_init(void)
{
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOC);

    gpio_mode_setup(SD_PWR_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SD_PWR_PIN);
    gpio_mode_setup(EPAPER_PWR_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, EPAPER_PWR_PIN);
    gpio_mode_setup(ESP_PWR_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, ESP_PWR_PIN);

    power_sd(false);
    power_epaper(false);
    power_esp(false);
}

void power_sd(bool enable) {
    if (enable) {
        gpio_clear(SD_PWR_PORT, SD_PWR_PIN);
    } else {
        gpio_set(SD_PWR_PORT, SD_PWR_PIN);
    }
}

void power_epaper(bool enable) {
    if (enable) {
        gpio_clear(EPAPER_PWR_PORT, EPAPER_PWR_PIN);
    } else {
        gpio_set(EPAPER_PWR_PORT, EPAPER_PWR_PIN);
    }
}

void power_esp(bool enable) {
    if (enable) {
        gpio_clear(ESP_PWR_PORT, ESP_PWR_PIN);
    } else {
        gpio_set(ESP_PWR_PORT, ESP_PWR_PIN);
    }
}

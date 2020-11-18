#ifndef RTC_H
#define RTC_H

#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/cm3/nvic.h>

void rtc_init(void);
void rtc_set_wakeup(unsigned int period);

#endif /* RTC_H */

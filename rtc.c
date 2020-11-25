#include "rtc.h"
#include <libopencm3/stm32/rcc.h>

void rtc_init(void)
{
    /* turn on power block to enable unlocking */
    rcc_periph_clock_enable(RCC_PWR);
    pwr_disable_backup_domain_write_protect();

    /* reset rtc */
    RCC_CSR |= RCC_CSR_RTCRST;
    RCC_CSR &= ~RCC_CSR_RTCRST;

    /* We want to use the LSE fitted on the discovery board */
    rcc_osc_on(RCC_LSE);
    rcc_wait_for_osc_ready(RCC_LSE);

    /* Select the LSE as rtc clock */
    rcc_rtc_select_clock(RCC_CSR_RTCSEL_LSE);

    /* ?! Stdperiph examples don't turn this on until _afterwards_ which
        * simply doesn't work.  It must be on at least to be able to
        * configure it */
    RCC_CSR |= RCC_CSR_RTCEN;

    rtc_unlock();

    /* enter init mode */
    RTC_ISR |= RTC_ISR_INIT;
    while ((RTC_ISR & RTC_ISR_INITF) == 0);

    /* set synch prescaler, using defaults for 1Hz out */
    uint32_t sync = 255;
    uint32_t async = 127;
    rtc_set_prescaler(sync, async);

    /* load time and date here if desired, and hour format */

    /* exit init mode */
    RTC_ISR &= ~(RTC_ISR_INIT);

    /* and write protect again */
    rtc_lock();

    /* and finally enable the clock */
    RCC_CSR |= RCC_CSR_RTCEN;

    /* And wait for synchro.. */
    rtc_wait_for_synchro();


    /* EXTI configuration */
    /* Configure the EXTI subsystem. */
    /* not needed, this chooses ports
    ⁞  exti_select_source(EXTI20, BUTTON_DISCO_USER_PORT)
    */
    exti_set_trigger(EXTI20, EXTI_TRIGGER_RISING);
    exti_enable_request(EXTI20);
}

void rtc_set_wakeup(unsigned int period)
{
    rtc_unlock();

    /* ensure wakeup timer is off */
    RTC_CR &= ~RTC_CR_WUTE;

    /* Wait until we can write */
    while ((RTC_ISR & RTC_ISR_WUTWF) == 0);

    /* Use the 1Hz clock as source */
    RTC_CR &= ~(RTC_CR_WUCLKSEL_MASK << RTC_CR_WUCLKSEL_SHIFT);

    if (period >= 65535) {
        // TODO description
        RTC_CR |= (0b110 << RTC_CR_WUCLKSEL_SHIFT);
        period -= 65536;
    } else {
        RTC_CR |= (RTC_CR_WUCLKSEL_SPRE << RTC_CR_WUCLKSEL_SHIFT);
    }

    RTC_WUTR = period - 1;


    /* Restart WakeUp unit */
    RTC_CR |= RTC_CR_WUTE;

    /* interrupt configuration */

    /* also, let's have an interrupt */
    RTC_CR |= RTC_CR_WUTIE;

    /* done with rtc registers, lock them again */
    rtc_lock();

    nvic_enable_irq(NVIC_RTC_IRQ);
}

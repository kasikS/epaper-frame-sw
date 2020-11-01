/*
 * serial.c
 *
 *  Created on: Sep 14, 2019
 *      Author: kasik
 */

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "serial.h"

/**
 * \brief Converts an integer to array of chars
 *
 * \param number is an integer to be converted
 * \param stringArray is an output array
 */
uint32_t toa(uint32_t number, char stringArray[])
{
	uint32_t temp = 0;
	uint32_t digits = 0;
	uint32_t i = 0;

	if (number == 0)
	{
		stringArray[0] = '0';
		digits++;
	}

	while(number!=0)
	{
		stringArray[digits] = '0' + number % 10;
		number = number/10;
		digits++;
	}

	for(i=0; i< (digits/2); i++)
	{
		temp = stringArray[i];
		stringArray[i] = stringArray[digits-1-i];
		stringArray[digits-1-i] = temp;
	}

	return digits;
}

/**
 * \brief Converts an integer to array of bits
 *
 * \param number is an integer to be converted
 * \param stringArray is an output array
 */
void to_bit_string(uint32_t number, char stringArray[])
{
	uint32_t mask = 0;
	uint32_t i = 0;

    mask =0;
    for(i=0; i<16; i++)
    {
    	mask= 1 << (16-i-1);
    	stringArray[i] = (number & mask)? '1':'0';
    }
}

/**
 * \brief Initializes USART1
 *
 * \param baud specifies the baud rate
 */
void serial_init(uint32_t baud)
{
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_USART1);


    /* Setup GPIO pins for USART2 transmit. */
    /* Setup USART2 TX pin as alternate function. */
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);
    gpio_set_af(GPIOA, GPIO_AF7, GPIO9);

    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO10);
    gpio_set_af(GPIOA, GPIO_AF7, GPIO10);

    usart_set_baudrate(USART1, baud);
    usart_set_databits(USART1, 8);
    usart_set_stopbits(USART1, USART_STOPBITS_1);
    usart_set_parity(USART1, USART_PARITY_NONE);
    usart_set_mode(USART1, USART_MODE_TX_RX);
    usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
    usart_enable(USART1);
}

/**
 * \brief Sends a character over USART1
 *
 * \param c is a character to be sent
 */
void serial_putc(const char c)
{
		/* sends a single character*/
		usart_send_blocking(USART1, c);
}

/**
 * \brief Sends a string over USART
 *
 * \param string is a pointer to the string to be sent
 * \param len is the number of characters to be sent
 */
void serial_write(const char *string, int len)
{
	int i;
	for(i=0; i<len; i++)
	{
		/* Puts data into buffer, sends the data*/
		serial_putc(*string);
		++string;
	}
}

void serial_puts(const char *string)
{
    while(*string)
    {
        serial_putc(*string);
        ++string;
    }
}

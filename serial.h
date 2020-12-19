/*
 * serial.h
 *
 *  Created on: Sep 14, 2019
 *      Author: kasik
 */

#ifndef SERIAL_H_
#define SERIAL_H_

uint32_t toa(uint32_t number, char stringArray[]);
void to_bit_string(uint32_t number, char stringArray[]);
void serial_init(uint32_t baud);
void serial_putc(const char c);
void serial_write(const char *string, int len);
void serial_puts(const char *string);
char serial_getc(void);

#endif /* SERIAL_H_ */

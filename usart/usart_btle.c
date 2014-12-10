/*
 * usart_btle.c
 *
 *  Created on: Oct 20, 2014
 *      Author: jcobb
 */

//#define BAUD 9600
#define BAUD 38400
//#define BAUD 57600
//#define BAUD 115200
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include "../util/clock.h"
#include "usart_btle.h"

#include <util/setbaud.h>
BTLE_BUFFER btle_buffer = {{0},0,0};

const unsigned char BTLE_hex[] PROGMEM = "0123456789ABCDEF";

btle_rx_cb_t btle_rx_cb;

void _btle_set_rx_cb(btle_rx_cb_t cb)
{
	btle_rx_cb = cb;
}

void btle_usart_init()
{
	UBRR1H = UBRRH_VALUE;
	UBRR1L = UBRRL_VALUE;

	// Enble receiver and transmitter
	UCSR1B |= (1<<RXCIE1) | (1<<TXEN1);

	// Set rx and tx enable bits
	UCSR1B |= (1 << RXEN1) | (1 << TXEN1);
	// Set databits to 8
	UCSR1C |= (1 << UCSZ11) | (1 << UCSZ10);
}

// not in use at this time
void btle_usart_init_cb(btle_rx_cb_t cb)
{
	UBRR1H = UBRRH_VALUE;
	UBRR1L = UBRRL_VALUE;

	// Enble receiver and transmitter
	UCSR1B |= (1<<RXCIE1) | (1<<TXEN1);

	// Set rx and tx enable bits
	UCSR1B |= (1 << RXEN1) | (1 << TXEN1);
	// Set databits to 8
	UCSR1C |= (1 << UCSZ11) | (1 << UCSZ10);

	_btle_set_rx_cb(cb);
}

void btle_usart_put_char(unsigned char c)
{
	int i = (unsigned int)(btle_buffer.head + 1) % BTLE_RX_BUFFER_SIZE;

	// if we should be storing the received character into the location
	// just before the tail (meaning that the head would advance to the
	// current location of the tail), we're about to overflow the buffer
	// and so we don't write the character or advance the head.
	if (i != btle_buffer.tail) {
		btle_buffer.buffer[btle_buffer.head] = c;
		btle_buffer.head = i;
	}
}


void btle_usart_clear_buffer()
{
	memset(&btle_buffer, 0, sizeof(BTLE_BUFFER));
}

uint8_t btle_usart_data_available()
{
	return (uint8_t)(BTLE_RX_BUFFER_SIZE + btle_buffer.head - btle_buffer.tail) % BTLE_RX_BUFFER_SIZE;
}

uint8_t btle_usart_data_read(void)
{
	// if the head isn't ahead of the tail, we don't have any characters
	if (btle_buffer.head == btle_buffer.tail) {
		return -1;
	} else {
		uint8_t c = btle_buffer.buffer[btle_buffer.tail];
		btle_buffer.tail = (unsigned int)(btle_buffer.tail + 1) % BTLE_RX_BUFFER_SIZE;
		return c;
	}
}

void btle_usart_transmit(uint8_t data )
{
	while (!( UCSR1A & (1<<UDRE1)));
	UDR1 = data;
}

void btle_usart_transmit_bytes(char data[], int size)
{
	for (int i=0;i<size;i++)
	{
		while (!( UCSR1A & (1<<UDRE1)));
		UDR1 = data[i];
	}
}

void btle_usart_transmit_string(char * data)
{
	unsigned char c = *data;

	while (c) {
		while (!( UCSR1A & (1<<UDRE1)));
		UDR1 = c;
		c = *(++data);
	}
}

ISR(BTLE_ISR_VECTOR)
{
	char data = UDR1;
	//if (btle_rx_cb != 0) btle_rx_cb(data);
	btle_usart_put_char(data);
	//PORTD ^= _BV(PD7);

	// debug callback to main program
	if (btle_rx_cb != 0)
	{
		btle_rx_cb(data);
	}
}

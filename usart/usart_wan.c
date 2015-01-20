/*
 * usart_wan.c
 *
 *  Created on: Nov 5, 2014
 *      Author: titan
 */

//#define BAUD 9600
#define BAUD 38400
//#define BAUD 57600
//#define BAUD 115200

#include <string.h>
#include <avr/io.h>
#include <util/setbaud.h>
#include <avr/interrupt.h>
#include "usart_wan.h"
#include "../wan/wan_msg.h"
#include "usart_btle.h"

WAN_BUFFER wan_buffer = { { 0 }, 0, 0 };

void wan_usart_init()
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

	// Enble receiver and transmitter
	UCSR0B |= (1 << RXCIE0) | (1 << TXEN0);

	// Set rx and tx enable bits
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
	// Set databits to 8
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
}

void wan_usart_put_char(unsigned char c)
{
	int i = (unsigned int) (wan_buffer.head + 1) % WAN_RX_BUFFER_SIZE;

	// if we should be storing the received character into the location
	// just before the tail (meaning that the head would advance to the
	// current location of the tail), we're about to overflow the buffer
	// and so we don't write the character or advance the head.
	if (i != wan_buffer.tail)
	{
		wan_buffer.buffer[wan_buffer.head] = c;
		wan_buffer.head = i;
	}
}

void wan_usart_clear_buffer()
{
	memset(&wan_buffer, 0, sizeof(WAN_BUFFER));
}

uint8_t wan_usart_data_available(void)
{
	return (uint8_t) (WAN_RX_BUFFER_SIZE + wan_buffer.head - wan_buffer.tail)
			% WAN_RX_BUFFER_SIZE;
}

uint8_t wan_usart_data_read(void)
{
	// if the head isn't ahead of the tail, we don't have any characters
	if (wan_buffer.head == wan_buffer.tail)
	{
		return -1;
	} else
	{
		uint8_t c = wan_buffer.buffer[wan_buffer.tail];
		wan_buffer.tail = (unsigned int) (wan_buffer.tail + 1)
				% WAN_RX_BUFFER_SIZE;
		return c;
	}
}

void wan_usart_transmit(uint8_t data)
{
	while (!(UCSR0A & (1 << UDRE0)))
		;
	UDR0 = data;
}

void wan_usart_transmit_bytes(char data[], int size)
{
	for (int i = 0; i < size; i++)
	{
		while (!(UCSR0A & (1 << UDRE0)))
			;
		UDR0 = data[i];
	}
}

void wan_usart_transmit_string(char * data)
{
	unsigned char c = *data;

	while (c)
	{
		while (!(UCSR0A & (1 << UDRE0)))
			;
		UDR0 = c;
		c = *(++data);
	}
}

ISR(WAN_ISR_VECTOR)
{
	unsigned char data = UDR0;
	//if (btle_rx_cb != 0) btle_rx_cb(data);
	wan_usart_put_char(data);

}

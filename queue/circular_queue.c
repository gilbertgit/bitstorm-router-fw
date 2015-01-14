/*
 * circular_queue.c
 *
 *  Created on: Jan 13, 2015
 *      Author: titan
 */

#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "circular_queue.h"

CIRCULAR_QUEUE circular_queue = {{0},0,0};

void circular_queue_put_char(uint8_t c)
{
	int i = (unsigned int)(circular_queue.head + 1) % CIRCULAR_QUEUE_SIZE;

	if (i != circular_queue.tail) {
		circular_queue.buffer[circular_queue.head] = c;
		circular_queue.head = i;
	}
}


void circular_queue_clear_buffer()
{
	memset(&circular_queue, 0, sizeof(CIRCULAR_QUEUE));
}

uint8_t circular_queue_data_available()
{
	return (uint8_t)(CIRCULAR_QUEUE_SIZE + circular_queue.head - circular_queue.tail) % CIRCULAR_QUEUE_SIZE;
}

uint8_t circular_queue_data_read(void)
{
	// if the head isn't ahead of the tail, we don't have any characters
	if (circular_queue.head == circular_queue.tail) {
		return -1;
	} else {
		uint8_t c = circular_queue.buffer[circular_queue.tail];
		circular_queue.tail = (unsigned int)(circular_queue.tail + 1) % CIRCULAR_QUEUE_SIZE;
		return c;
	}
}

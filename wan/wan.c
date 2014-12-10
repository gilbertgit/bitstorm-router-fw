/*
 * wan.c
 *
 *  Created on: Nov 5, 2014
 *      Author: titan
 */

#include <string.h>
#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "../util/defines.h"
#include "../queue/queue.h"
#include "../usart/usart_wan.h"
#include "wan.h"
#include "wan_msg.h"
#include "wan_driver.h"
#include "wan_config.h"

// queue management
queue_t wan_queue;
bool frame_ready = false;
static uint8_t frame_buff[80];
static int frame_index = 0;
static int frame_length = 0;
static bool config_finished = false;
enum states {
	CONFIGURE, RUNNING
};
static uint8_t state = CONFIGURE;

void wan_init()
{
	wan_usart_init();

	wan_driver_init();
	// configure init
}

void wan_set_cts()
{
	//pd0 low
	PORTD &= ~_BV(PD0);
}

uint8_t wan_get_rts()
{
	// return logic high or low
	return (PIND & _BV(PD1));
}

void wan_tick()
{
	if (!config_finished)
			config_finished = wan_config_tick();

	frame_tick();

	if (frame_ready)
	{
		if (state == CONFIGURE)
		{
			config_finished = wan_config_received(frame_buff);

		}
		frame_ready = false;
		frame_index = 0;
	}

	// check to see if we have a new message
	if (wan_queue.count > 0)
	{

		queue_header_t *qh;
		qh = wan_queue.head;

		wan_msg_t *msg = (wan_msg_t *) QUEUE_DATA(qh);
		// TODO: Handle Messages
		// do something with the message

		// Dequeue the message
		queue_remove(&wan_queue, (queue_header_t*) msg);

	}
}

void frame_tick()
{
	// do some sanaty checks on buffer overruns
	while (wan_usart_data_available())
	{
		frame_buff[frame_index] = wan_usart_data_read();
		if (frame_index == 0)
		{
			frame_length = frame_buff[frame_index];
		}
		else if (frame_index >= frame_length)
		{
			frame_ready = true;
			while (wan_usart_data_available())
				wan_usart_data_read();
			break;
		}

		frame_index++;
	}
}

queue_results_t wan_enqueue(wan_msg_t *msg)
{
	//queue_results_t result = queue_enqueue(&wan_queue, &msg, sizeof(wan_msg_t));
	queue_results_t result = queue_enqueue(&wan_queue, msg, sizeof(wan_msg_t));

	return result;
}

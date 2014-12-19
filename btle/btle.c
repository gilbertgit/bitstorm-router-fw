/*
 * btle.c
 *
 *  Created on: Nov 3, 2014
 *      Author: jcobb
 */

#include <string.h>
#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "../util/defines.h"
#include "../queue/queue.h"
#include "../usart/usart_btle.h"
#include "btle.h"
#include "btle_msg.h"
#include "btle_driver.h"
#include "../usart/usart_wan.h"
#include "../wan/wan_msg.h"
#include "../ramdisk/ramdisk.h"
#include "../shared.h"

// queue management
queue_t btle_queue;

void btle_init()
{
	//btle_usart_init();
	// set portd bit 5 as output
	DDRD |= _BV(PD5);
	// set portd bit 4 as input
	DDRD &= ~_BV(PD4);

	btle_driver_init();
	btle_set_cts();
}

void btle_set_cts()
{
	//pd5 low
	PORTD &= ~_BV(PD5);
}

uint8_t btle_get_rts()
{
	// return logic high or low
	return (PIND & _BV(PD4));
}

void build_app_msg(btle_msg_t *btle_msg, app_msg_t *msg)
{

	msg->messageType = 1;
	msg->nodeType = 1;
	msg->extAddr = btle_msg->mac;
	msg->shortAddr = shared.mac & 0x0000FFFF;
	msg->routerAddr = shared.mac;
	//softVersion;
	//channelMask;
	msg->panId = 0x1973; // need to set pan in zigbit
	msg->workingChannel = 0x16;
	msg->parentShortAddr = 1;
	msg->lqi = 0;

	msg->rssi = btle_msg->rssi;
	msg->battery = btle_msg->batt;
	msg->temperature = btle_msg->temp;

}

void btle_tick()
{
	btle_driver_tick();
	// check to see if we have a new message
	btle_msg_t *msg = ramdisk_next(NULL );
	if (msg != NULL )
	{
		app_msg_t app_msg;
		cmd_send_header_t cmd_header;
		uint8_t frame[80];

		build_app_msg(msg, &app_msg);

		// TODO: Handle Messages
		// push out the lw-mesh radio
		//if (!(PINB & (1 << PB0)))
		//{

			frame[0] = sizeof(cmd_header) + sizeof(app_msg) + 1;
			cmd_header.command = CMD_SEND;
			cmd_header.pan_id = 0x1973;
			cmd_header.short_id = 0x0000;
			cmd_header.message_length = sizeof(app_msg);

			int frame_index = 1;
			// header
			for (int i = 0; i < sizeof(cmd_header); i++)
			{
				frame[frame_index++] = ((uint8_t *) (&cmd_header))[i];
			}
			// message
			for (int i = 0; i < sizeof(app_msg_t); i++)
			{
				frame[frame_index++] = ((uint8_t *) (&app_msg))[i];
			}
			// checksum
			frame[frame_index++] = 0xFF;

			wan_usart_transmit_bytes((char*) frame, frame_index);
			PORTD ^= _BV(PD7);
		//}
		// Dequeue the message
		ramdisk_erase(*msg);

	}
}

queue_results_t btle_enqueue(btle_msg_t *msg)
{
	//queue_results_t result = queue_enqueue(&btle_queue, &msg, sizeof(btle_msg_t));
	queue_results_t result = queue_enqueue(&btle_queue, msg,
			sizeof(btle_msg_t));

	return result;
}


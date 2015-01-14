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
#include "../util/clock.h"
#include "../usart/usart_wan.h"
#include "../wan/wan_msg.h"
#include "../ramdisk/ramdisk.h"
#include "../shared.h"
#include "../queue/circular_queue.h"

// queue management
queue_t btle_queue;
queue_t packet_queue;

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

	msg->messageType = 0x01;
	msg->nodeType = 0x01;
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
#ifdef BYPASSS_MODE
#else
	ramdisk_clean_tick();
#endif

	// check to see if we have a new message
	if ((PINB & (1 << PB0)))
	{
		if(circular_queue_data_available())
		{
		//queue_header_t *qh;
		//qh = packet_queue.head;

		//btle_msg_t *msg = (btle_msg_t *) QUEUE_DATA(qh);
		btle_msg_t *msg;
		uint8_t temp[sizeof(btle_msg_t)];
		for(int i = 0; i < sizeof(btle_msg_t); i++)
		{
			temp[i] = circular_queue_data_read();
		}
		msg = (btle_msg_t *) temp;
		if (msg != NULL)
		{
			app_msg_t app_msg;
			cmd_send_header_t cmd_header;
			uint8_t frame[80];

			build_app_msg(msg, &app_msg);

			// TODO: Handle Messages
			// push out the lw-mesh radio

			frame[0] = sizeof(cmd_header) + sizeof(app_msg) + 1;

			if (msg->type == MSG_TYPE_IN_PROX)
				app_msg.messageType = CMD_IN_PROX;
			else if (msg->type == MSG_TYPE_OUT_PROX)
				app_msg.messageType = CMD_OUT_PROX;

#ifdef ZB_ACK
			cmd_header.command = CMD_ACK_SEND;
#else
			cmd_header.command = CMD_SEND;
#endif
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

			// Dequeue the message
			//queue_remove(&packet_queue, (queue_header_t*) msg);
		}
	}
	}
}

queue_results_t btle_enqueue(btle_msg_t *msg)
{
	//queue_results_t result = queue_enqueue(&btle_queue, &msg, sizeof(btle_msg_t));
	queue_results_t result = queue_enqueue(&btle_queue, msg,
			sizeof(btle_msg_t));

	return result;
}


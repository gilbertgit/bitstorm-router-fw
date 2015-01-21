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
#include "../util/clock.h"
#include "../queue/circular_queue.h"
#include "../btle/btle.h"
#include "../btle/btle_msg.h"
#include "../btle/btle_driver.h"
#include "../shared.h"

#define NWK_READY (PINB & (1 << PB0))
#define NWK_BUSY  (~(PINB & (1 << PB0)))

#define LED_OFF	{ PORTD |= _BV(PD7); }
#define LED_ON	{ PORTD &= ~_BV(PD7); }

// queue management
queue_t wan_queue;
bool frame_ready = false;
static uint8_t frame_buff[80];
static int frame_index = 0;
static int frame_length = 0;
static clock_time_t prev_millis;

enum states {
	CONFIGURE, WAIT_FOR_DATA, WAIT_FOR_NWK_BUSY, WAIT_FOR_NWK_READY
};
static uint8_t state = CONFIGURE;

void wan_init() {
	wan_usart_init();

	wan_driver_init();
}

void wan_set_cts() {
	//pd0 low
	PORTD &= ~_BV(PD0);
}

uint8_t wan_get_rts() {
	// return logic high or low
	return (PIND & _BV(PD1));
}

void wan_reset_frame(void) {
	frame_ready = false;
	frame_index = 0;
}

void wan_state_configure(void) {
	if (wan_config_tick()) {
		state = WAIT_FOR_DATA;
	} else if (frame_ready) {
		if (wan_config_received(frame_buff)) {
			state = WAIT_FOR_DATA;
		}
		wan_reset_frame();
	}
}

void wan_state_wait_for_data(void) {
	// check if we have data to send
	if (circular_queue_data_available()) {
		// check to see if we can send (network not busy)
		if (NWK_READY) {

			btle_msg_t *msg;
			uint8_t temp[sizeof(btle_msg_t)];
			for (int i = 0; i < sizeof(btle_msg_t); i++) {
				temp[i] = circular_queue_data_read();
			}
			msg = (btle_msg_t *) temp;

			app_msg_t app_msg;
			cmd_send_header_t cmd_header;
			uint8_t frame[80];

			build_app_msg(msg, &app_msg);

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
			for (int i = 0; i < sizeof(cmd_header); i++) {
				frame[frame_index++] = ((uint8_t *) (&cmd_header))[i];
			}
			// message
			for (int i = 0; i < sizeof(app_msg_t); i++) {
				frame[frame_index++] = ((uint8_t *) (&app_msg))[i];
			}
			// checksum
			uint8_t cs = 0;
			for (int i = 0; i < frame_index; cs ^= frame[i++]);
			frame[frame_index++] = cs;

			// push out the lw-mesh radio
			wan_usart_transmit_bytes((char*) frame, frame_index);

			state = WAIT_FOR_NWK_BUSY;
			prev_millis = clock_time();
			LED_ON
		}
	}
}

void frame_tick() {
	// do some sanity checks on buffer overruns
	while (wan_usart_data_available()) {
		frame_buff[frame_index] = wan_usart_data_read();
		if (frame_index == 0) {
			frame_length = frame_buff[frame_index];
		} else if (frame_index >= frame_length) {
			frame_ready = true;
			while (wan_usart_data_available())
				wan_usart_data_read();
			break;
		}

		frame_index++;
	}
}

void wan_tick() {

	clock_time_t elapsed;

	frame_tick();

	switch (state) {
	case CONFIGURE:
		wan_state_configure();
		break;
	case WAIT_FOR_DATA:
		wan_state_wait_for_data();
		break;
	case WAIT_FOR_NWK_BUSY:
		if (NWK_BUSY) {
			state = WAIT_FOR_NWK_READY;
			prev_millis = clock_time();
		} else {
			// Don't wait around forever - perhaps the RF sent quickly and we missed the bounce
			elapsed = clock_time() - prev_millis;
			if (elapsed >= WAN_NWK_BUSY_TIMEOUT) {
				state = WAIT_FOR_NWK_READY;
				prev_millis = clock_time();
			}
		}
		break;
	case WAIT_FOR_NWK_READY:
		if (NWK_READY) {
			state = WAIT_FOR_DATA;
			LED_OFF
		} else {
			// Don't wait forever - although, if it never goes to READY, there's not much we can do...
			elapsed = clock_time() - prev_millis;
			if (elapsed >= WAN_NWK_READY_TIMEOUT) {
				state = WAIT_FOR_DATA;
				LED_OFF
			}
		}
		break;
	}
}

void build_app_msg(btle_msg_t *btle_msg, app_msg_t *msg) {

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

	// Calculate CS
	//msg->cs = 0;
	//for (int i = 0; i < sizeof(app_msg_t) - 1; msg->cs ^= ((uint8_t*) msg)[i++])
	//	;

	// ACTUALLY, don't calculate the CS, let the frame cs do the trick
	msg->cs = 0xCC;
}

queue_results_t wan_enqueue(wan_msg_t *msg) {
	//queue_results_t result = queue_enqueue(&wan_queue, &msg, sizeof(wan_msg_t));
	queue_results_t result = queue_enqueue(&wan_queue, msg, sizeof(wan_msg_t));

	return result;
}

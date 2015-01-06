/*
 * btle_driver.c

 *
 *  Created on: Nov 4, 2014
 *      Author: jcobb
 */

#include <string.h>
#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "../util/defines.h"
#include "../util/clock.h"
#include "../queue/queue.h"
#include "../usart/usart_btle.h"
#include "btle_driver.h"
#include "btle.h"
#include "btle_msg.h"
#include "../usart/usart_wan.h"
#include "../ramdisk/ramdisk.h"

char HEX_DIGITS[] = "0123456789abcdef";

#define BTLE_MAX_CHARS		128

char btle_lines[BTLE_MAX_CHARS + 1];
int btle_index = 0;
uint8_t new_line = 0;

char btle_line_buffer[BTLE_MAX_CHARS + 1];
int btle_line_index = 0;

static void init_buffer();
static void init_lines();
static bool handle_data();
static uint8_t parse_data(const char *token, char **out);

static uint8_t btle_parse_nybble(char c);
static btle_msg_t btle_handle_le_packet(char * buffer);

static btle_msg_t *next_msg = NULL;

void btle_driver_init()
{
	queue_init(&btle_queue, MSG_QUEUE_SIZE);
	queue_init(&packet_queue, MSG_QUEUE_SIZE);
	init_buffer();
	init_lines();
}

void btle_driver_tick()
{

	if (btle_usart_data_available())
	{

		if (handle_data())
		{

			char *ptr = NULL;
			// handle the new line

			if (parse_data((const char*) BTLE_TKSTART, &ptr) == BTLE_TKFOUND)
			{
				// TODO: handle the message
				btle_msg_t msg = btle_handle_le_packet(ptr);

				if (msg.mac != 0)
				{
					btle_msg_t *temp = ramdisk_find(msg.mac);

					// no package in ramdisk
					if (temp == NULL )
					{
						// new packet
						msg.last_sent = clock_time();
						msg.count = 1;
						ramdisk_write(msg);
						enqueue_packet(MSG_TYPE_IN_PROX, &msg);

						// send in proximity
					} else
					{
						// package in RAM
						// update packet
						temp->rssi = msg.rssi;
						temp->batt = msg.batt;
						temp->temp = msg.temp;
						temp->count = temp->count + 1;

						// is the packet stale?
						if ((clock_time() - temp->last_sent) > 5000)
						{
							// send standard packet
							enqueue_packet(MSG_TYPE_NORM, temp);
							temp->last_sent = clock_time();
						}

					}
				}
			}
		}
	}
}

void enqueue_packet(uint8_t msg_type, btle_msg_t *msg)
{
	msg->type = msg_type;
	queue_enqueue(&packet_queue, msg, sizeof(btle_msg_t));
}

void ramdisk_clean_tick()
{
	btle_msg_t *msg;
	if (next_msg != NULL)
	{
		msg = next_msg;
		next_msg = ramdisk_next(next_msg);
		if ((clock_time() - msg->last_sent) >= 10000)
		{
			//send "is out of prox packet"
			enqueue_packet(MSG_TYPE_OUT_PROX, msg);
			// erase the packet
			ramdisk_erase(*msg);
		}

	}
	else
	{
		next_msg = ramdisk_next(NULL);
	}

}

void encode_string(btle_msg_t * value);

#define DEBUG_OUTPUT "rssi=%d batt=%d temp=%d mac=%d \r\n\0"
void encode_string(btle_msg_t * value)
{
	char tmp[50];

	memset(tmp, '\0', 50);
	sprintf_P(tmp, PSTR(DEBUG_OUTPUT), value->rssi, value->batt, value->temp,
			value->mac);
	//LOG("%s\r\n", tmp);
	wan_usart_transmit_string(tmp);
}

static void init_buffer()
{
	btle_line_index = 0;
	memset(btle_line_buffer, '\0', sizeof(btle_line_buffer));
}

static void init_lines()
{
	memset(btle_lines, '\0', sizeof(btle_lines));
}

// check to see if we have a new line
bool handle_data()
{

	char c = btle_usart_data_read();

	// ignore null terminated strings
	if (c == '\0')
		return false;
	// prevent buffer overrun
	if (btle_line_index >= BTLE_MAX_CHARS)
		return false;

	// store character in btle_line_buffer
	btle_line_buffer[btle_line_index] = c;
	btle_line_index++;

	// check for end of line
	if (c == BTLE_TKEND[0])
	{
		// copy new message into buffer
		strcpy(btle_lines, btle_line_buffer);
		init_buffer();
		return true;
	}

	return false;
}

static uint8_t parse_data(const char *token, char **out)
{
	char* ptr = NULL;
	if ((ptr = strstr(btle_lines, token)))
	{
		if (out != NULL )
			*out = ptr;
		return BTLE_TKFOUND;
	} else
		return BTLE_TKNOTFOUND;
}

btle_msg_t btle_handle_le_packet(char * buffer)
{
	btle_msg_t btle_msg;

	memset(&btle_msg, 0, sizeof(btle_msg_t));

	//           1111111111222222222
	// 01234567890123456789012345678
	// |||||||||||||||||||||||||||||
	// *00078072CCB3 C3 5994 63BC 24

	uint8_t * num;
	uint8_t msb, lsb, ck, ckx;
	uint8_t rssi;
	uint16_t batt, temp;
	uint64_t mac;
	int i;

	// Validate checksum in bytes 27-28
	// Just an XOR of bytes 0-26
	msb = btle_parse_nybble(buffer[27]);
	lsb = btle_parse_nybble(buffer[28]);
	ck = (msb << 4) | lsb;
	ckx = 0;
	for (i = 0; i <= 26; i++)
		ckx ^= buffer[i];
	if (ck != ckx)
	{
		return btle_msg;
	}

	// MAC address - incoming 48bits
	//
	num = (uint8_t *) &mac;
	num[7] = 0;
	num[6] = 0;
	msb = btle_parse_nybble(buffer[1]);
	lsb = btle_parse_nybble(buffer[2]);
	num[5] = (msb << 4) | lsb;
	msb = btle_parse_nybble(buffer[3]);
	lsb = btle_parse_nybble(buffer[4]);
	num[4] = (msb << 4) | lsb;
	msb = btle_parse_nybble(buffer[5]);
	lsb = btle_parse_nybble(buffer[6]);
	num[3] = (msb << 4) | lsb;
	msb = btle_parse_nybble(buffer[7]);
	lsb = btle_parse_nybble(buffer[8]);
	num[2] = (msb << 4) | lsb;
	msb = btle_parse_nybble(buffer[9]);
	lsb = btle_parse_nybble(buffer[10]);
	num[1] = (msb << 4) | lsb;
	msb = btle_parse_nybble(buffer[11]);
	lsb = btle_parse_nybble(buffer[12]);
	num[0] = (msb << 4) | lsb;

	// RSSI
	//
	msb = btle_parse_nybble(buffer[14]);
	lsb = btle_parse_nybble(buffer[15]);
	rssi = (msb << 4) | lsb;

	// Temperature
	//
	num = (uint8_t *) &temp;
	msb = btle_parse_nybble(buffer[17]);
	lsb = btle_parse_nybble(buffer[18]);
	num[0] = (msb << 4) | lsb;
	msb = btle_parse_nybble(buffer[19]);
	lsb = btle_parse_nybble(buffer[20]);
	num[1] = (msb << 4) | lsb;

	// Battery
	//
	num = (uint8_t *) &batt;
	msb = btle_parse_nybble(buffer[22]);
	lsb = btle_parse_nybble(buffer[23]);
	num[0] = (msb << 4) | lsb;
	msb = btle_parse_nybble(buffer[24]);
	lsb = btle_parse_nybble(buffer[25]);
	num[1] = (msb << 4) | lsb;

	btle_msg.rssi = rssi;
	btle_msg.mac = mac;
	btle_msg.batt = batt;
	btle_msg.temp = temp;

	return btle_msg;

}

uint8_t btle_parse_nybble(char c)
{
	if (c >= 'A' && c <= 'F')
		c = c | 0x20;
	for (uint8_t i = 0; i < 16; i++)
	{
		if (HEX_DIGITS[i] == c)
			return i;
	}
	return 0x80;
}


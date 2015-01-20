/*
 * wan_driver.c
 *
 *  Created on: Nov 5, 2014
 *      Author: titan
 */

#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "../util/defines.h"
#include "../queue/queue.h"
#include "../usart/usart_wan.h"
#include "../usart/usart_btle.h"
#include "wan_driver.h"
#include "wan.h"
#include "wan_msg.h"
#include "wan_config.h"

char WAN_HEX_DIGITS[] = "0123456789abcdef";

#define WAN_MAX_CHARS		128

char wan_lines[WAN_MAX_CHARS + 1];
int wan_index = 0;
uint8_t wan_new_line = 0;

char wan_line_buffer[WAN_MAX_CHARS + 1];
int wan_line_index = 0;

static void init_buffer();
static void init_lines();
//static bool handle_data();
//static uint8_t parse_data(char *token, char **out);

//static uint8_t wan_parse_nybble(char c);
//static wan_msg_t wan_handle_packet(char * buffer);

void wan_driver_init()
{
	//queue_init(&wan_queue, WAN_MSG_QUEUE_SIZE);
	init_buffer();
	init_lines();
}

void wan_driver_tick()
{
	if (wan_usart_data_available())
	{
		//wan_config_tick();
	}
}

static void init_buffer()
{
	wan_line_index = 0;
	memset(wan_line_buffer, '\0', sizeof(wan_line_buffer));
}

static void init_lines()
{
	memset(wan_lines, '\0', sizeof(wan_lines));
}

//bool handle_data()
//{
//
//	char c = wan_usart_data_read();
//
//	// ignore null terminated strings
//	if (c == '\0')
//		return false;
//	// prevent buffer overrun
//	if (wan_line_index >= WAN_MAX_CHARS)
//		return false;
//
//	// store character in btle_line_buffer
//	wan_line_buffer[wan_line_index] = c;
//	wan_line_index++;
//
//	// check for end of line
//	if (c == WAN_TKEND)
//	{
//		// copy new message into buffer
//		strcpy(wan_lines, wan_line_buffer);
//		init_buffer();
//		return true;
//	}
//
//	return false;
//}

//static uint8_t parse_data(char *token, char **out)
//{
//	uint8_t *ptr = NULL;
//	if ((ptr == strstr(wan_lines, token)))
//	{
//		if (out != NULL )
//			*out = ptr;
//		return WAN_TKFOUND;
//	} else
//		return WAN_TKNOTFOUND;
//}

//wan_msg_t wan_handle_packet(char * buffer)
//{
//	wan_msg_t wan_msg;
//
//	return wan_msg;
//
//}
//
//uint8_t wan_parse_nybble(char c)
//{
//	if (c >= 'A' && c <= 'F')
//		c = c | 0x20;
//	for (uint8_t i = 0; i < 16; i++)
//	{
//		if (WAN_HEX_DIGITS[i] == c)
//			return i;
//	}
//	return 0x80;
//}

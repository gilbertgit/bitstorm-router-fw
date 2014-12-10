/*
 * bt_msg.h
 *
 *  Created on: Nov 3, 2014
 *      Author: jcobb
 */

#ifndef BT_MSG_H_
#define BT_MSG_H_

#include "../sys/sysTypes.h"

#define MSG_SIZE				64 // index(1) + origin(11) + 1 + content(50) + 1 + type(1) + state(1) = 66


typedef struct
{
	uint8_t		rssi;
	uint64_t	mac;
	uint16_t	batt;
	uint16_t	temp;
	struct btle_msg_t *next;

} btle_msg_t;

typedef struct PACK app_msg_t
{
	uint8_t     messageType;
	uint8_t     nodeType;
	uint64_t    extAddr;
	uint16_t    shortAddr;
	uint64_t		routerAddr;
	//uint32_t    softVersion;
	//uint32_t    channelMask;
	uint16_t    panId;
	uint8_t     workingChannel;
	uint16_t    parentShortAddr;
	uint8_t     lqi;
	int8_t      rssi;
	uint8_t			ackByte;

	int32_t   battery;
	int32_t   temperature;

	uint8_t cs;

} app_msg_t;


typedef struct
{
	uint8_t command;
	uint16_t pan_id;
	uint8_t short_id;
	uint8_t message_length;
}cmd_send_header_t;


#endif /* BT_MSG_H_ */

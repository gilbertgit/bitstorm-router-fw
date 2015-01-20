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

typedef struct {
	uint8_t rssi;
	uint64_t mac;
	uint16_t batt;
	uint16_t temp;
	struct btle_msg_t *next;
	uint32_t last_sent;
		uint8_t count;
		uint8_t type;

} btle_msg_t;

typedef struct {
	uint8_t command;
	uint16_t pan_id;
	uint8_t short_id;
	uint8_t message_length;
} cmd_send_header_t;

#endif /* BT_MSG_H_ */

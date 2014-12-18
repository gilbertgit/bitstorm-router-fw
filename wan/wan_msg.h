/*
 * wan_msg.h
 *
 *  Created on: Nov 5, 2014
 *      Author: titan
 */

#ifndef WAN_MSG_H_
#define WAN_MSG_H_

#define MSG_SIZE				64 // index(1) + origin(11) + 1 + content(50) + 1 + type(1) + state(1) = 66


typedef struct
{
	uint8_t		rssi;
	uint64_t	mac;
	uint16_t	batt;
	uint16_t	temp;

} wan_msg_t;

typedef struct
{
	uint8_t command;
}cmd_header_t;

typedef struct
{
	uint8_t command;
	uint16_t pan_id;
	uint16_t short_id;
	uint8_t channel;
}cmd_config_ntw_t;

enum
{
	CMD_SEND = 0x01,
	CMD_ACK_SEND = 0x02,
	CMD_CONFIG_NETWORK = 0x03,
	CMD_GET_ADDRESS = 0x04

};


#endif /* WAN_MSG_H_ */

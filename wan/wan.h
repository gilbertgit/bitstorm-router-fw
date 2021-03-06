/*
 * wan.h
 *
 *  Created on: Nov 5, 2014
 *      Author: titan
 */

#ifndef WAN_H_
#define WAN_H_

#include "wan_msg.h"
#include"../btle/btle_msg.h"
//#include "../queue/queue.h"
// btle parsing results
enum wan_parse_result {
 WAN_TKNOTFOUND = 0,
 WAN_TKFOUND,
 WAN_TKERROR,
 WAN_TKTIMEOUT
};

// token start and end
#define WAN_TKSTART	'*'
#define WAN_TKEND		'\n'
#define WAN_MSG_QUEUE_SIZE			MSG_SIZE * 128

// Shouldn't take too long to recognize a BUSY signal
#define WAN_NWK_BUSY_TIMEOUT	100

// For ACK'd messages, it can take up to 3 full seconds
#define WAN_NWK_READY_TIMEOUT	4000

extern queue_t wan_queue;

void wan_init();
void wan_tick();
queue_results_t wan_enqueue(wan_msg_t *msg);
void wan_set_cts();//pd5 output and low
uint8_t wan_get_rts();//pd4 input
void frame_tick();
void build_app_msg(btle_msg_t *btle_msg, app_msg_t *msg);

#endif /* WAN_H_ */

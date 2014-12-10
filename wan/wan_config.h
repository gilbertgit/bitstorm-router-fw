/*
 * wan_config.h
 *
 *  Created on: Dec 4, 2014
 *      Author: titan
 */

#ifndef WAN_CONFIG_H_
#define WAN_CONFIG_H_

#include "wan_received_types.h"

void wan_get_device_address();
bool wan_config_tick();
void config_mac_resp(mac_resp_t * resp);
bool wan_config_received(uint8_t * buff);
void config_ntw_resp(config_ntw_resp_t * resp);

#endif /* WAN_CONFIG_H_ */

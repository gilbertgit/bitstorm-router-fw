/*
 * btle_driver.h
 *
 *  Created on: Nov 4, 2014
 *      Author: jcobb
 */

#ifndef BTLE_DRIVER_H_
#define BTLE_DRIVER_H_

#include "btle_msg.h"

void btle_driver_init();
void btle_driver_tick();
void enqueue_packet(uint8_t msg_type, btle_msg_t *msg);
void ramdisk_clean_tick();

static uint8_t MSG_TYPE_NORM = 1;
static uint8_t MSG_TYPE_IN_PROX = 5;
static uint8_t MSG_TYPE_OUT_PROX = 6;

#endif /* BTLE_DRIVER_H_ */

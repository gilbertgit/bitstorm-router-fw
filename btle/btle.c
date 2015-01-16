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
#include "btle.h"
#include "btle_msg.h"
#include "btle_driver.h"
#include "../util/clock.h"

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

void btle_tick()
{
	btle_driver_tick();
#ifdef BYPASSS_MODE
#else
	ramdisk_clean_tick();
#endif

}

queue_results_t btle_enqueue(btle_msg_t *msg)
{
	//queue_results_t result = queue_enqueue(&btle_queue, &msg, sizeof(btle_msg_t));
	queue_results_t result = queue_enqueue(&btle_queue, msg,
			sizeof(btle_msg_t));

	return result;
}


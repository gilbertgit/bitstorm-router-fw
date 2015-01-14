/*
 * circular_queue.h
 *
 *  Created on: Jan 13, 2015
 *      Author: titan
 */

#ifndef CIRCULAR_QUEUE_H_
#define CIRCULAR_QUEUE_H_

#define CIRCULAR_QUEUE_SIZE 	512

typedef struct
{
	unsigned char buffer[CIRCULAR_QUEUE_SIZE];
	int head;
	int tail;
} CIRCULAR_QUEUE;

extern CIRCULAR_QUEUE circular_queue;
void circular_queue_put_char(uint8_t c);
void circular_queue_clear_buffer();
uint8_t circular_queue_data_available(void);
uint8_t circular_queue_data_read(void);

#endif /* CIRCULAR_QUEUE_H_ */

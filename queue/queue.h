/*
 * queue.h
 *
 *  Created on: Nov 3, 2014
 *      Author: jcobb
 */

#ifndef QUEUE_H_
#define QUEUE_H_



typedef enum
{
	QUEUE_UNKNOWN = 0,
	QUEUE_NO_MEMORY,
	QUEUE_OVERFLOW,
	QUEUE_EMPTY,
	QUEUE_NOT_FOUND,
	QUEUE_SUCCESS = 0xFF
} queue_results_t;

typedef struct
{
	void * prev;
	void * next;
	uint16_t length;
} queue_header_t;

typedef struct
{
	uint16_t max_size;
	uint16_t count;
	queue_header_t * head;
	queue_header_t * tail;
	uint16_t data_usage;
	uint16_t memory_usage;
} queue_t;

void queue_init(queue_t *, uint16_t max_size);
queue_results_t queue_enqueue(queue_t *, void *, uint16_t);
queue_results_t queue_dequeue(queue_t * r, void ** dest);
queue_results_t queue_find(queue_t * q, void * src, uint16_t length, queue_header_t ** dest);
queue_results_t queue_remove(queue_t * q, queue_header_t * x);
queue_results_t queue_peek_length(queue_t *, uint16_t *);
queue_results_t queue_clear(queue_t *);

#define QUEUE_DATA(h)	((void*)h + sizeof(queue_header_t))

#endif /* QUEUE_H_ */

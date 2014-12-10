/*
 * queue.c
 *
 *  Created on: Nov 3, 2014
 *      Author: jcobb
 */
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include "queue.h"


// Calculate memory usage as: DATABYTES + ALL_HEADERBYTES + ALL_MALLOCHEADERBYTES
#define MEMORY_CALC(r)		(r->data_usage + (r->count * sizeof(queue_header_t)) + (r->count * 4));


void queue_reset(queue_t * q)
{
	q->head = 0;
	q->tail = 0;
	q->count = 0;
	q->data_usage = 0;
	q->memory_usage = 0;
}

void queue_init(queue_t * r, uint16_t max)
{
	r->max_size = max;
	queue_reset(r);
}

queue_results_t queue_enqueue(queue_t * r, void * data, uint16_t length)
{
	queue_header_t header;
	uint16_t new_size;

	if (r->max_size)
	{
		// See if we have room: CURRENTMEMORYBYTES + DATABYTES + ALL_HEADERBYTES + ALL_MALLOCHEADERBYTES
		new_size = r->memory_usage + length + sizeof(queue_header_t) + 4;
		if (new_size > r->max_size) return QUEUE_OVERFLOW;
	}

	// Try to grab some space
	void * obj = malloc(sizeof(queue_header_t) + length);

	// Bail if we couldn't get the memory
	if (obj == 0) return QUEUE_NO_MEMORY;

	// New tail header
	header.length = length;
	header.prev = 0;
	header.next = 0;

	// If the queue is empty,
	if (r->head == 0)
	{
		// ... add this single object, and it doesn't point to anything
		r->head = obj;
		r->tail = obj;
		r->count = 1;
		r->data_usage = header.length;
		header.next = 0;
	}
	else
	{
		// ... otherwise, the tail grows. Make this the new tail and bump the counter
		header.prev = r->tail;
		r->tail->next = (queue_header_t *)obj;
		r->tail = (queue_header_t *)obj;
		r->count++;
		r->data_usage += header.length;
	}
	r->memory_usage = MEMORY_CALC(r);

	// Now build out the header and blast the data into memory
	memcpy(obj, &header, sizeof(queue_header_t));
	memcpy(QUEUE_DATA(obj), data, length);

	return QUEUE_SUCCESS;
}

queue_results_t queue_dequeue(queue_t * r, void ** dest)
{
	queue_header_t * header;

	// Notify that the queue is empty
	if (r->head == 0) return QUEUE_EMPTY;

	// Reference the header
	header = (queue_header_t *)r->head;

	// Grab the data
	*dest = (void*)malloc(header->length);
	if (dest == 0) return QUEUE_NO_MEMORY;
	memcpy(*dest, QUEUE_DATA(header), header->length);

	// If this was the only object, empty the queue
	if (header->next == 0)
	{
		r->head = 0;
		r->tail = 0;
		r->count = 0;
		r->data_usage = 0;
	}
	else
	{
		// Ring still has objects, so set the new head and adjust the count
		r->head = header->next;
		r->count--;
		r->data_usage -= header->length;
		((queue_header_t*)r->head)->prev = 0;
	}
	r->memory_usage = MEMORY_CALC(r);

	// Finally, free the memory
	free((void*) header);

	return QUEUE_SUCCESS;
}

queue_results_t queue_find(queue_t * q, void * src, uint16_t length, queue_header_t ** dest)
{
	queue_header_t * h;
	uint8_t comp = 0;

	if (q->count == 0)
	{
		*dest = 0;
		return QUEUE_EMPTY;
	}

	h = q->head;
	while (h)
	{
		comp = memcmp(QUEUE_DATA(h), src, length);
		if (comp == 0)
		{
			*dest = h;
			return QUEUE_SUCCESS;
		}
		h = h->next;
	}

	// If we got here, it wasn't found
	return QUEUE_NOT_FOUND;
}

queue_results_t queue_remove(queue_t * q, queue_header_t * x)
{
	if (q->count == 0)
	{
		return QUEUE_EMPTY;
	}
	else if (q->count == 1)
	{
		free((void*)x);
		queue_reset(q);
		return QUEUE_SUCCESS;
	}
	else
	{
		if ((queue_header_t *)(x->prev))
			((queue_header_t *)(x->prev))->next = x->next;
		else
			q->head = x->next;

		if ((queue_header_t *)(x->next))
			((queue_header_t *)(x->next))->prev = x->prev;
		else
			q->tail = x->prev;

		q->count--;
		q->data_usage -= x->length;
		q->memory_usage = MEMORY_CALC(q);
		free((void*)x);
		return QUEUE_SUCCESS;
	}
}

queue_results_t queue_peek_length(queue_t * r, uint16_t * length)
{
	if (r->head == 0) return QUEUE_EMPTY;
	*length = ((queue_header_t *)r->head)->length;
	return QUEUE_SUCCESS;
}

queue_results_t queue_clear(queue_t * r)
{
	queue_header_t * obj;
	queue_header_t * next;

	obj = r->head;
	while (obj)
	{
		next = obj->next;
		free((void *)obj);
		obj = next;
	}

	queue_reset(r);

	return QUEUE_SUCCESS;
}

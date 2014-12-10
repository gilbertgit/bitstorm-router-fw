/*
 * log.h
 *
 *  Created on: Oct 20, 2014
 *      Author: jcobb
 */

#ifndef USART_BTLE_H_
#define USART_BTLE_H_

typedef void (*btle_rx_cb_t)(uint8_t);
typedef void (*btle_out_cb_t)(uint8_t);

#define BTLE_RX_BUFFER_SIZE 	512
#define BTLE_ISR_VECTOR			USART1_RX_vect

typedef struct
{
	unsigned char buffer[BTLE_RX_BUFFER_SIZE];
	int head;
	int tail;
} BTLE_BUFFER;

extern BTLE_BUFFER btle_buffer;

void btle_usart_init();
void btle_usart_init_cb(btle_rx_cb_t cb);
void btle_usart_put_char(unsigned char c);
void btle_usart_clear_buffer();
uint8_t btle_usart_data_available(void);
uint8_t btle_usart_data_read(void);
void btle_usart_transmit(uint8_t data );
void btle_usart_transmit_bytes(char data[], int size);
void btle_usart_transmit_string(char * data);

#endif /* USART_BTLE_H_ */

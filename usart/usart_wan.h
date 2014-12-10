/*
 * usart_wan.h
 *
 *  Created on: Nov 5, 2014
 *      Author: titan
 */

#ifndef USART_WAN_H_
#define USART_WAN_H_

typedef void (*wan_rx_cb_t)(uint8_t);
typedef void (*wan_out_cb_t)(uint8_t);

#define WAN_RX_BUFFER_SIZE 	512
#define WAN_ISR_VECTOR			USART0_RX_vect

typedef struct
{
	unsigned char buffer[WAN_RX_BUFFER_SIZE];
	int head;
	int tail;
} WAN_BUFFER;

void wan_usart_init();
void wan_usart_init_cb(wan_rx_cb_t cb);
void wan_usart_put_char(unsigned char c);
void wan_usart_clear_buffer();
uint8_t wan_usart_data_available(void);
uint8_t wan_usart_data_read(void);
void wan_usart_transmit(uint8_t data );
void wan_usart_transmit_bytes(char data[], int size);
void wan_usart_transmit_string(char * data);


#endif /* USART_WAN_H_ */

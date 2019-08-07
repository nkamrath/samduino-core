#ifndef _DRIVERS_USART_H_
#define _DRIVERS_USART_H_

#include <asf.h>
#include <stdbool.h>

typedef void* usart_t;

typedef struct
{
	void* dev_ptr;
	uint32_t baud_rate;
	void* callback_arg;
	uint32_t parity;
	uint32_t stop_bits;
	void (*tx_ready_callback)(void* arg, void** next_tx_buffer, uint32_t* next_tx_buffer_length);
	void (*rx_ready_callback)(void* arg, void* rx_data, uint32_t rx_length, void** next_rx_buffer, uint32_t* next_rx_buffer_length);
} usart_params_t;

usart_t Usart_Create(usart_params_t* params);
bool Usart_Destroy(usart_t dev);

bool Usart_DmaWrite(usart_t port, void* buffer, uint32_t buffer_length);
bool Usart_DmaRead(usart_t port, void* buffer, uint32_t buffer_length);

void Usart_StopWrite(usart_t port, void** buffer, uint32_t* buffer_length);
void Usart_StopRead(usart_t port, void** buffer, uint32_t* buffer_length);

#endif
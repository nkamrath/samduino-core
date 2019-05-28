#ifndef _DRIVERS_UART_H_
#define _DRIVERS_UART_H_

#include <asf.h>
#include <stdbool.h>

typedef void* uart_t;

typedef struct
{
	void* dev_ptr;
	uint32_t baud_rate;
	void (*tx_ready_callback)(uart_t dev, void** next_tx_buffer, uint32_t* next_tx_buffer_length);
	void (*rx_ready_callback)(uart_t dev, void* rx_data, uint32_t rx_length, void** next_rx_buffer, uint32_t* next_rx_buffer_length);
} uart_params_t;

uart_t Uart_Create(uart_params_t* params);
bool Uart_Destroy(uart_t dev);

bool Uart_DmaWrite(uart_t port, void* buffer, uint32_t buffer_length);
bool Uart_DmaRead(uart_t port, void* buffer, uint32_t buffer_length);

#endif
#include "pa6h.h"

#define _RX_BUFFER_SIZE_BYTES 8
#define _TX_BUFFER_SIZE_BYTES 128
#define _RX_ASSEMBLY_BUFFER_SIZE_BYTES 256

typedef struct
{
	uart_t uart;
	uint8_t rx_buffers[2][_RX_BUFFER_SIZE_BYTES];
	uint8_t current_rx_buffer_index;

	uint8_t tx_buffer[_TX_BUFFER_SIZE_BYTES];

	uint8_t parse_buffer[_RX_ASSEMBLY_BUFFER_SIZE_BYTES];
	uint16_t parse_buffer_index;



}_pa6h_t;

void _TxCallback(void* arg, void** next_tx_buffer, uint32_t* next_tx_buffer_length)
{
	_pa6h_t* dev = (_pa6h_t*)arg;

	*next_tx_buffer = NULL;
	*next_tx_buffer_length = 0;
}

void _RxCallback(void* arg, void* rx_data, uint32_t rx_length, void** next_rx_buffer, uint32_t* next_rx_buffer_length)
{
	_pa6h_t* dev = (_pa6h_t*)arg;

	dev->current_rx_buffer_index ^= 1;

	*next_rx_buffer = dev->rx_buffers[dev->current_rx_buffer_index];
	*next_rx_buffer_length = _RX_BUFFER_SIZE_BYTES;
}

void _HandleRxData(_pa6h_t* dev)
{
	//figure out which buffer to copy from
	uint8_t* buffer = dev->rx_buffers[0];
	if(dev->current_rx_buffer_index == 0)
	{
		buffer = dev->rx_buffers[1];
	}

	//copy dma buffer to parse buffer so a message parse can be attempted
	memcpy(&dev->parse_buffer[dev->parse_buffer_index], buffer, _RX_BUFFER_SIZE_BYTES);
	uint16_t prev_parse_buffer_index = dev->parse_buffer_index;
	dev->parse_buffer_index += _RX_BUFFER_SIZE_BYTES;

	//starting at prev_parse_buffer_index, look for a nema message end character
}

pa6h_t Pa6h_Create(pa6h_params_t* params)
{
	_pa6h_t* dev = malloc(sizeof(_pa6h_t));
	if(dev)
	{
		params->uart_params.tx_ready_callback = _TxCallback;
		params->uart_params.rx_ready_callback = _RxCallback;
		params->uart_params.callback_arg = dev;

		dev->uart = Uart_Create(&params->uart_params);
	}

	return dev;
}
#include "uart.h"

typedef struct
{
	Uart* dev;
	uint32_t baud_rate;
	void (*tx_ready_callback)(uart_t dev, void** next_tx_buffer, uint32_t* next_tx_buffer_length);
	void (*rx_ready_callback)(uart_t dev, void* rx_data, uint32_t rx_length, void** next_rx_buffer, uint32_t* next_rx_buffer_length);
	Pdc* pdc_base;
	pdc_packet_t tx_dma_packet;
	pdc_packet_t rx_dma_packet;
} _uart_t;

static _uart_t _uart0;
static _uart_t _uart1;

void _SetupRxDma(uart_t port, void* buffer, uint32_t length)
{
	_uart_t* interface = (_uart_t*)port;

	interface->rx_dma_packet.ul_addr = (uint32_t)buffer;
	interface->rx_dma_packet.ul_size = length;

	pdc_rx_init(interface->pdc_base, &interface->rx_dma_packet, NULL);
	uart_enable_interrupt(interface->dev, UART_IER_RXBUFF);
	pdc_enable_transfer(interface->pdc_base, PERIPH_PTCR_RXTEN);
}

void _SetupTxDma(uart_t port, void* buffer, uint32_t length)
{
	_uart_t* interface = (_uart_t*)port;

	interface->tx_dma_packet.ul_addr = (uint32_t)buffer;
	interface->tx_dma_packet.ul_size = length;

	pdc_tx_init(interface->pdc_base, &interface->tx_dma_packet, NULL);
	uart_enable_interrupt(interface->dev, UART_IER_TXEMPTY);
	pdc_enable_transfer(interface->pdc_base, PERIPH_PTCR_TXTEN);
}

void UART0_Handler()
{
	void* next_buffer = NULL;
	uint32_t next_buffer_length = 0;

	uint32_t uart_status = uart_get_status(UART0);
	if(uart_status & (UART_SR_RXRDY | UART_IER_RXBUFF))
	{
		void* rx_ptr = (void*)pdc_read_rx_ptr(_uart0.pdc_base);
		uint32_t rx_length = pdc_read_rx_counter(_uart0.pdc_base);

		_uart0.rx_ready_callback(UART0, rx_ptr, rx_length, &next_buffer, &next_buffer_length);
		if(next_buffer && next_buffer_length > 0)
		{
			_SetupRxDma(&_uart0, next_buffer, next_buffer_length);
		}
	}
	
	if(uart_status & UART_SR_TXEMPTY)
	{
		_uart0.tx_ready_callback(UART0, &next_buffer, &next_buffer_length);
		if(next_buffer && next_buffer_length)
		{
			_SetupTxDma(&_uart0, next_buffer, next_buffer_length);
		}
	}
}

void UART1_Handler()
{
	void* next_buffer = NULL;
	uint32_t next_buffer_length = 0;

	uint32_t uart_status = uart_get_status(UART1);
	if(uart_status & (UART_SR_RXRDY | UART_IER_RXBUFF))
	{
		void* rx_ptr = (void*)pdc_read_rx_ptr(_uart1.pdc_base);
		uint32_t rx_length = pdc_read_rx_counter(_uart1.pdc_base);

		_uart1.rx_ready_callback(UART1, rx_ptr, rx_length, &next_buffer, &next_buffer_length);
		if(next_buffer && next_buffer_length > 0)
		{
			_SetupRxDma(&_uart1, next_buffer, next_buffer_length);
		}
	}
	
	if(uart_status & UART_SR_TXEMPTY)
	{
		_uart1.tx_ready_callback(UART1, &next_buffer, &next_buffer_length);
		if(next_buffer && next_buffer_length)
		{
			_SetupTxDma(&_uart1, next_buffer, next_buffer_length);
		}
	}
}

uart_t Uart_Create(uart_params_t* params)
{
	IRQn_Type uart_irq_num;
	int uart_id;
	Pdc* pdc_base = NULL;
	_uart_t* interface = NULL;
	//setup the UART peripheral
	if(params->dev_ptr == UART0)
	{
		//set pins for uart0 peripheral, peripheral A RXD = PA9, TXD = PA10
		pio_configure(PIOA, PIO_PERIPH_A, (PIO_PA9A_URXD0 | PIO_PA10A_UTXD0), PIO_DEFAULT);
		//set the uart irq number
		uart_irq_num = UART0_IRQn;
		uart_id = ID_UART0;
		pdc_base = PDC_UART0;
		interface = &_uart0;
	}
	else if(params->dev_ptr == UART1)
	{
		//set pins for uart1 peripheral
		pio_configure(PIOB, PIO_PERIPH_A, (PIO_PB2A_URXD1 | PIO_PB3A_UTXD1), PIO_DEFAULT);
		uart_irq_num = UART1_IRQn;
		uart_id = ID_UART1;
		pdc_base = PDC_UART1;
		interface = &_uart1;
	}
	else
	{
		return NULL; //error, uart_peripheral not set to correct base address
	}

	interface->dev = params->dev_ptr;
	interface->tx_ready_callback = params->tx_ready_callback;
	interface->rx_ready_callback = params->rx_ready_callback;
	interface->baud_rate = params->baud_rate;
	interface->pdc_base = pdc_base;

	//setup the options for the uart
	sam_uart_opt_t uart_settings = 
	{
		sysclk_get_cpu_hz(),
		params->baud_rate,
		UART_MR_PAR_NO
	};

	//enable clock gate to peripheral
	sysclk_enable_peripheral_clock(uart_id);

	//init the uart peripheral
	uart_init(interface->dev, &uart_settings);

	//enable the rx interrupt
	//uart_enable_interrupt(interface->dev, UART_IER_RXRDY);
	NVIC_EnableIRQ(uart_irq_num);

	return interface;
}

bool Uart_DmaWrite(uart_t port, void* buffer, uint32_t buffer_length)
{
	_SetupTxDma(port, buffer, buffer_length);
	return true;
}

bool Uart_DmaRead(uart_t port, void* buffer, uint32_t buffer_length)
{
	_SetupRxDma(port, buffer, buffer_length);
	return true;
}
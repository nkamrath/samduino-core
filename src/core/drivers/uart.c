#include "uart.h"

typedef struct
{
	Uart* dev;
	uint32_t baud_rate;
	void (*tx_ready_callback)(uart_t dev);
	void (*rx_ready_callback)(uart_t dev);
	Pdc* pdc_base;
	pdc_packet_t tx_dma_packet;
	pdc_packet_t rx_dma_packet;
} _uart_t;

static _uart_t _uart0;
static _uart_t _uart1;

void UART0_Handler()
{
	uint32_t uart_status = uart_get_status(UART0);
	if(uart_status & (UART_SR_RXRDY | UART_IER_RXBUFF))
	{
		_uart0.rx_ready_callback(UART0);
	}
	
	if(uart_status & UART_SR_TXEMPTY)
	{
		_uart0.tx_ready_callback(UART0);
	}
}

void UART1_Handler()
{
	uint32_t uart_status = uart_get_status(UART1);
	if(uart_status & (UART_SR_RXRDY | UART_IER_RXBUFF))
	{
		_uart1.rx_ready_callback(UART1);
	}
	
	if(uart_status & UART_SR_TXEMPTY)
	{
		_uart1.tx_ready_callback(UART1);
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
	_uart_t* interface = (_uart_t*)port;

	interface->tx_dma_packet.ul_addr = (uint32_t)buffer;
	interface->tx_dma_packet.ul_size = buffer_length;

	pdc_tx_init(interface->pdc_base, &interface->tx_dma_packet, NULL);
	uart_enable_interrupt(interface->dev, UART_IER_TXEMPTY);
	pdc_enable_transfer(interface->pdc_base, PERIPH_PTCR_TXTEN);

	return true;
}

bool Uart_DmaRead(uart_t port, void* buffer, uint32_t buffer_length)
{
	_uart_t* interface = (_uart_t*)port;

	interface->rx_dma_packet.ul_addr = (uint32_t)buffer;
	interface->rx_dma_packet.ul_size = buffer_length;

	pdc_rx_init(interface->pdc_base, &interface->rx_dma_packet, NULL);
	uart_enable_interrupt(interface->dev, UART_IER_RXBUFF);
	pdc_enable_transfer(interface->pdc_base, PERIPH_PTCR_RXTEN);

	return true;
}
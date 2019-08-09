#include "usart.h"

typedef struct
{
	Usart* dev;
	uint32_t baud_rate;
	void* callback_arg;
	void (*tx_ready_callback)(usart_t dev, void** next_tx_buffer, uint32_t* next_tx_buffer_length);
	void (*rx_ready_callback)(usart_t dev, void* rx_data, uint32_t rx_length, void** next_rx_buffer, uint32_t* next_rx_buffer_length);
	Pdc* pdc_base;
	pdc_packet_t tx_dma_packet;
	pdc_packet_t rx_dma_packet;
} _usart_t;

static _usart_t _usart0;
static _usart_t _usart1;

static void _SetupRxDma(usart_t port, void* buffer, uint32_t length)
{
	_usart_t* interface = (_usart_t*)port;

	interface->rx_dma_packet.ul_addr = (uint32_t)buffer;
	interface->rx_dma_packet.ul_size = length;

	pdc_rx_init(interface->pdc_base, &interface->rx_dma_packet, NULL);
	usart_enable_interrupt(interface->dev, US_IER_RXBUFF);
	pdc_enable_transfer(interface->pdc_base, PERIPH_PTCR_RXTEN);
}

static void _SetupTxDma(usart_t port, void* buffer, uint32_t length)
{
	_usart_t* interface = (_usart_t*)port;

	interface->tx_dma_packet.ul_addr = (uint32_t)buffer;
	interface->tx_dma_packet.ul_size = length;

	pdc_tx_init(interface->pdc_base, &interface->tx_dma_packet, NULL);
	usart_enable_interrupt(interface->dev, US_IER_TXEMPTY);
	pdc_enable_transfer(interface->pdc_base, PERIPH_PTCR_TXTEN);
}

void USART0_Handler()
{
	void* next_buffer = NULL;
	uint32_t next_buffer_length = 0;

	uint32_t usart_status = usart_get_status(USART0);
	if(usart_status & (US_CSR_RXRDY | US_IER_RXBUFF))
	{
		void* rx_ptr = (void*)pdc_read_rx_ptr(_usart0.pdc_base);
		uint32_t rx_length = pdc_read_rx_counter(_usart0.pdc_base);

		_usart0.rx_ready_callback(_usart0.callback_arg, rx_ptr, rx_length, &next_buffer, &next_buffer_length);
		if(next_buffer && next_buffer_length > 0)
		{
			_SetupRxDma(&_usart0, next_buffer, next_buffer_length);
		}
		else
		{
			usart_disable_interrupt(_usart0.dev, US_IDR_RXBUFF);
		}
	}
	
	if(usart_status & US_CSR_TXEMPTY)
	{
		_usart0.tx_ready_callback(_usart0.callback_arg, &next_buffer, &next_buffer_length);
		if(next_buffer && next_buffer_length)
		{
			_SetupTxDma(&_usart0, next_buffer, next_buffer_length);
		}
		else
		{
			usart_disable_interrupt(_usart0.dev, US_IDR_TXEMPTY);
		}
	}
}

void USART1_Handler()
{
	void* next_buffer = NULL;
	uint32_t next_buffer_length = 0;

	uint32_t usart_status = usart_get_status(USART1);
	if(usart_status & (US_CSR_RXRDY | US_IER_RXBUFF))
	{
		void* rx_ptr = (void*)pdc_read_rx_ptr(_usart1.pdc_base);
		uint32_t rx_length = pdc_read_rx_counter(_usart1.pdc_base);

		_usart1.rx_ready_callback(_usart1.callback_arg, rx_ptr, rx_length, &next_buffer, &next_buffer_length);
		if(next_buffer && next_buffer_length > 0)
		{
			_SetupRxDma(&_usart1, next_buffer, next_buffer_length);
		}
		else
		{
			usart_disable_interrupt(_usart1.dev, US_IDR_RXBUFF);
		}
	}
	
	if(usart_status & US_CSR_TXEMPTY)
	{
		_usart1.tx_ready_callback(_usart1.callback_arg, &next_buffer, &next_buffer_length);
		if(next_buffer && next_buffer_length)
		{
			_SetupTxDma(&_usart1, next_buffer, next_buffer_length);
		}
		else
		{
			usart_disable_interrupt(_usart1.dev, US_IDR_TXEMPTY);
		}
	}
}

usart_t Usart_Create(usart_params_t* params)
{
	IRQn_Type usart_irq_num;
	int usart_id;
	Pdc* pdc_base = NULL;
	_usart_t* interface = NULL;
	//setup the USART peripheral
	if(params->dev_ptr == USART0)
	{
		//set pins for usart0 peripheral, peripheral A RXD = PA9, TXD = PA10
		pio_configure(PIOA, PIO_PERIPH_A, (PIO_PA5A_RXD0 | PIO_PA6A_TXD0), PIO_DEFAULT);
		//set the usart irq number
		usart_irq_num = USART0_IRQn;
		usart_id = ID_USART0;
		pdc_base = PDC_USART0;
		interface = &_usart0;
	}
	else if(params->dev_ptr == USART1)
	{
		//set pins for usart1 peripheral
		pio_configure(PIOA, PIO_PERIPH_A, (PIO_PA21A_RXD1 | PIO_PA22A_TXD1), PIO_DEFAULT);
		usart_irq_num = USART1_IRQn;
		usart_id = ID_USART1;
		pdc_base = PDC_USART1;
		interface = &_usart1;
	}
	else
	{
		return NULL; //error, usart_peripheral not set to correct base address
	}

	interface->dev = params->dev_ptr;
	interface->tx_ready_callback = params->tx_ready_callback;
	interface->rx_ready_callback = params->rx_ready_callback;
	interface->baud_rate = params->baud_rate;
	interface->pdc_base = pdc_base;
	interface->callback_arg = params->callback_arg;

	//setup the options for the usart
	sam_usart_opt_t usart_settings = 
	{
		
		params->baud_rate,
		US_MR_CHRL_8_BIT,
		params->parity,
		params->stop_bits,
		US_MR_CHMODE_NORMAL
	};

	//enable clock gate to peripheral
	sysclk_enable_peripheral_clock(usart_id);

	//init the usart peripheral
	usart_init_rs232(interface->dev, &usart_settings, sysclk_get_peripheral_hz());


	usart_disable_interrupt(interface->dev, US_IDR_RXBUFF);
	usart_disable_interrupt(interface->dev, US_IDR_TXEMPTY);
	usart_enable_tx(interface->dev);
	usart_enable_rx(interface->dev);

	//enable the rx interrupt
	//usart_enable_interrupt(interface->dev, USART_IER_RXRDY);
	NVIC_EnableIRQ(usart_irq_num);

	return interface;
}

bool Usart_DmaWrite(usart_t port, void* buffer, uint32_t buffer_length)
{
	_SetupTxDma(port, buffer, buffer_length);
	return true;
}

bool Usart_DmaRead(usart_t port, void* buffer, uint32_t buffer_length)
{
	_SetupRxDma(port, buffer, buffer_length);
	return true;
}

void Usart_StopWrite(usart_t port, void** buffer, uint32_t* buffer_length)
{
	_usart_t* interface = (_usart_t*)port;

	pdc_disable_transfer(interface->pdc_base, PERIPH_PTCR_TXTDIS);

	*buffer = (void*) pdc_read_tx_ptr(interface->pdc_base);
	*buffer_length = pdc_read_tx_counter(interface->pdc_base);
}

void Usart_StopRead(usart_t port, void** buffer, uint32_t* buffer_length)
{
	_usart_t* interface = (_usart_t*)port;

	pdc_disable_transfer(interface->pdc_base, PERIPH_PTCR_RXTDIS);

	*buffer = (void*) pdc_read_rx_ptr(interface->pdc_base);
	*buffer_length = pdc_read_rx_counter(interface->pdc_base);
}
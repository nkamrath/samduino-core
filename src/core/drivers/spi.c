#include "spi.h"
#include <asf.h>

typedef struct
{
	Spi* dev;
	uint32_t clk_hz;
	void (*transfer_complete_callback)(spi_t dev);
	Pdc* pdc_base;
	pdc_packet_t tx_control_dma_packet;
	pdc_packet_t tx_data_dma_packet;
	pdc_packet_t rx_control_dma_packet;
	pdc_packet_t rx_data_dma_packet;
} _spi_t;

static _spi_t _spi0;

spi_t Spi_Create(spi_params_t* params)
{
	_spi_t* interface = NULL;
	if(params->dev_ptr == SPI)
	{
		interface = &_spi0;
		interface->dev = params->dev_ptr;
		interface->transfer_complete_callback = params->transfer_complete_callback;
		interface->clk_hz = params->clk_hz;

		//configure spi pins
		pio_configure(PIOA, PIO_PERIPH_A, (PIO_PA14A_SPCK | PIO_PA13A_MOSI | PIO_PA12A_MISO), PIO_DEFAULT);
		sysclk_enable_peripheral_clock(ID_SPI);
		
		spi_reset(interface->dev);
		spi_set_master_mode(interface->dev);
		spi_disable_mode_fault_detect(interface->dev);
		spi_disable_loopback(interface->dev);
		//spi_set_peripheral_chip_select_value(spi, spi_get_pcs(0));  //sets the chip select value
		spi_disable_peripheral_select_decode(interface->dev);
		
		spi_set_bits_per_transfer(interface->dev, 0, SPI_CSR_BITS_8_BIT);
		spi_set_baudrate_div(interface->dev, 0, spi_calc_baudrate_div(interface->clk_hz, sysclk_get_cpu_hz()));
		spi_configure_cs_behavior(interface->dev, 0, SPI_CS_KEEP_LOW);
		spi_set_clock_polarity(interface->dev, params->polarity, 0);
		spi_set_clock_phase(interface->dev, params->phase, 1);

		interface->pdc_base = PDC_SPI;

		IRQn_Type spi_irq_num = SPI_IRQn;
		NVIC_EnableIRQ(spi_irq_num);
		
		spi_enable(interface->dev);
	}

	return interface;
}

bool Spi_Destroy(spi_t dev)
{
	return true;
}

bool Spi_DmaTransfer(spi_t port, void* tx_control, void* tx_data, void* rx_control, void* rx_data, uint32_t control_length, uint32_t data_length)
{
	_spi_t* interface = (_spi_t*)port;

	uint32_t transfer_enables = PERIPH_PTCR_TXTEN;

	interface->tx_control_dma_packet.ul_addr = (uint32_t)tx_control;
	interface->tx_control_dma_packet.ul_size = control_length;
	interface->tx_data_dma_packet.ul_addr = (uint32_t)tx_data;
	interface->tx_data_dma_packet.ul_size = data_length;

	if(rx_control && rx_data)
	{
		interface->rx_control_dma_packet.ul_addr = (uint32_t)rx_control;
		interface->rx_control_dma_packet.ul_size = control_length;
		interface->rx_data_dma_packet.ul_addr = (uint32_t)rx_data;
		interface->rx_data_dma_packet.ul_size = data_length;
		transfer_enables |= PERIPH_PTCR_RXTEN;
	}

	pdc_tx_init(interface->pdc_base, &interface->tx_control_dma_packet, &interface->tx_data_dma_packet);
	pdc_rx_init(interface->pdc_base, &interface->rx_control_dma_packet, &interface->rx_data_dma_packet);

	//spi_enable_interrupt(interface->dev, SPI_IER_TXEMPTY);

	pdc_enable_transfer(interface->pdc_base, transfer_enables);

	while((spi_read_status(_spi0.dev) & SPI_SR_TXEMPTY) == 0);

	pdc_disable_transfer(interface->pdc_base, PERIPH_PTCR_RXTDIS | PERIPH_PTCR_TXTDIS);

	return true;
}
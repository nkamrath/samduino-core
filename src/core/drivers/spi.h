#ifndef _DRIVERS_SPI_H_
#define _DRIVERS_SPI_H_

#include <asf.h>
#include <stdbool.h>

typedef void* spi_t;

typedef struct
{
	void* dev_ptr;
	uint32_t clk_hz;
	uint32_t polarity;
	uint32_t phase;
	void (*transfer_complete_callback)(spi_t dev);
} spi_params_t;

spi_t Spi_Create(spi_params_t* params);
bool Spi_Destroy(spi_t dev);

//rx control and data can be null if this is a write only operation.  Must always have a tx buffer
bool Spi_DmaTransfer(spi_t port, void* tx_control, void* tx_data, void* rx_control, void* rx_data, uint32_t control_length, uint32_t data_length);

#endif
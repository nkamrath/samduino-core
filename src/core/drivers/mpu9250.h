#ifndef _DRIVERS_MPU9250_H_
#define _DRIVERS_MPU9250_H_
#include <asf.h>
#include "drivers/spi.h"
#include "drivers/pin.h"

typedef void* mpu9250_t;

typedef struct
{
	spi_params_t spi_params;
	pin_params_t cs_pin_params;
	void* int_bank;
	uint32_t int_mask;
} mpu9250_params_t;

mpu9250_t Mpu9250_Create(mpu9250_params_t* params);

#endif
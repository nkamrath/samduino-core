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
	pin_params_t int_pin_params;
} mpu9250_params_t;

mpu9250_t Mpu9250_Create(mpu9250_params_t* params);
void Mpu9250_EnableInterrupts(mpu9250_t dev_ptr);
uint8_t Mpu9250_GetInterruptStatus(mpu9250_t dev_ptr);
void Mpu9250_ReadGyro(mpu9250_t dev_ptr, int16_t* values);
void Mpu9250_ReadAccel(mpu9250_t dev_ptr, int16_t* values);

#endif
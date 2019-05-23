#ifndef _DRIVERS_MPU9250_H_
#define _DRIVERS_MPU9250_H_
#include <asf.h>
#include "drivers/spi.h"
#include "drivers/pin.h"

typedef void* mpu9250_t;

typedef enum
{
	GYRO_SCALE__250DPS,
	GYRO_SCALE__500DPS,
	GYRO_SCALE__1000DPS,
	GYRO_SCALE__2000DPS,
} gyro_scale_t;

typedef enum
{
	ACCEL_SCALE__2G,
	ACCEL_SCALE__4G,
	ACCEL_SCALE__8G,
	ACCEL_SCALE__16G,
} accel_scale_t;

typedef struct
{
	spi_params_t spi_params;
	pin_params_t cs_pin_params;
	pin_params_t int_pin_params;
	gyro_scale_t gyro_scale;
	accel_scale_t accel_scale;
} mpu9250_params_t;

mpu9250_t Mpu9250_Create(mpu9250_params_t* params);
void Mpu9250_EnableInterrupts(mpu9250_t dev_ptr);
void Mpu9250_EnableDmp(mpu9250_t dev_ptr);
uint16_t Mpu9250_GetInterruptStatus(mpu9250_t dev_ptr);
void Mpu9250_ReadGyro(mpu9250_t dev_ptr, int16_t* values);
void Mpu9250_ReadAccel(mpu9250_t dev_ptr, int16_t* values);
bool Mpu9250_ReadQuat(mpu9250_t dev_ptr, int32_t* values);
void Mpu9250_SetGyroScale(mpu9250_t dev_ptr, gyro_scale_t scale);
void Mpu9250_SetAccelScale(mpu9250_t dev_ptr, accel_scale_t scale);

#endif
#ifndef _SENSORS_GPS_PA6H_H_
#define _SENSORS_GPS_PA6H_H_

#include "core/drivers/uart.h"

typedef struct
{
	uart_params_t uart_params;	
} pa6h_params_t;

typedef void* pa6h_t;

pa6h_t Pa6h_Create(pa6h_params_t* params);


#endif
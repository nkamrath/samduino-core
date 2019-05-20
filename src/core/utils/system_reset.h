#ifndef _UTILS_SYSTEM_RESET_H_
#define _UTILS_SYSTEM_RESET_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef enum
{
	SYSTEM_RESET_SOURCE__USB_DTR,
	SYSTEM_RESET_SOURCE__INVALID
} system_reset_source_t;

void SystemReset_Create(void);
void SystemReset_AddSource(system_reset_source_t);

#endif
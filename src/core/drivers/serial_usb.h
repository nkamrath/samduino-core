#ifndef _CORE_DRIVERS_SERIAL_USB_H_
#define _CORE_DRIVERS_SERIAL_USB_H_

#include <asf.h>
#include <stdint.h>
#include <stdbool.h>

uint32_t SerialUsb_Create(void);
uint32_t SerialUsb_Destroy(void);
bool SerialUsb_IsSetup(void);
uint32_t SerialUsb_RegisterDTRCallback(void(*callback)(uint8_t port, uint8_t set, void* arg), void* arg);
uint32_t SerialUsb_Write(char data);
uint32_t SerialUsb_WriteBuffer(char* buffer, uint32_t buffer_length);
uint32_t SerialUsb_ReadBuffer(char* buffer, uint32_t buffer_length);

#endif
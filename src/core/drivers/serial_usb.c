#include "serial_usb.h"

static bool _created = false;

static void(*_dtr_changed_callback)(uint8_t port, uint8_t set, void* arg) = NULL;
static void* _dtr_changed_callback_arg = NULL;

void _CleanupCallbacks(void)
{
	_dtr_changed_callback = NULL;
	_dtr_changed_callback_arg = NULL;
}

uint32_t SerialUsb_Create(void)
{
	if(!_created)
	{
		_CleanupCallbacks();
		stdio_usb_init();
		_created = true;
	}
	return 0;
}

uint32_t SerialUsb_Destroy(void)
{
	if(_created)
	{
		udc_stop();
		_created = false;
		_CleanupCallbacks();
		return 0;
	}
	return 1;
}

bool SerialUsb_IsSetup(void)
{
	return _created;
}

uint32_t SerialUsb_RegisterDTRCallback(void(*callback)(uint8_t port, uint8_t set, void* arg), void* arg)
{
	if(callback)
	{
		_dtr_changed_callback = callback;
		_dtr_changed_callback_arg = arg;
		return 0;
	}
	return 1;
}

uint32_t SerialUsb_Write(char data)
{
	if(udi_cdc_is_tx_ready())
	{
		return udi_cdc_putc(data & 0xff);
	}
	else
	{
		return 0;
	}
}

uint32_t SerialUsb_WriteBuffer(char* buffer, uint32_t buffer_length)
{
	int bytesSent = 0;
	for(size_t i = 0; i < buffer_length; i++)
	{
		//bytesSent += udi_cdc_putc(buffer[bytesSent]);
		//while(udi_cdc_is_tx_ready() == 0); //wait for tx to be ready to send
		//bytesSent += udi_cdc_putc(buffer[bytesSent]);
		bytesSent += SerialUsb_Write((uint8_t)(buffer[i] & 0xff));
	}
	return bytesSent;
}

uint32_t SerialUsb_ReadBuffer(char* buffer, uint32_t buffer_length)
{
	return udi_cdc_read_no_polling(buffer, buffer_length);
}

// size_t UsbSerial_Write(uint8_t data)
// {
// 	//while(udi_cdc_is_tx_ready() == 0); //wait for tx to be ready to send
// 	if(udi_cdc_is_tx_ready())
// 	{
// 		return udi_cdc_putc(data & 0xff);
// 	}
// 	else
// 	{
// 		return 0;
// 	}
// }

// size_t UsbSerial_WriteBuffer(const uint8_t* buffer, size_t bufferLength)
// {	
// 	int bytesSent = 0;
// 	for(size_t i = 0; i < bufferLength; i++)
// 	{
// 		//bytesSent += udi_cdc_putc(buffer[bytesSent]);
// 		//while(udi_cdc_is_tx_ready() == 0); //wait for tx to be ready to send
// 		//bytesSent += udi_cdc_putc(buffer[bytesSent]);
// 		bytesSent += UsbSerial_Write((uint8_t)(buffer[i] & 0xff));
// 	}
// 	return bytesSent;
// }


//below are callback functions registered with atmel usb stack for usb serial cdc device events
void usb_serial_tx_empty_notify(uint8_t port)
{
	
}

void usb_serial_dtr_changed(uint8_t port, uint8_t set)
{
	if(_dtr_changed_callback)
	{
		_dtr_changed_callback(port, set, _dtr_changed_callback_arg);
	}
}

void usb_serial_rx_notify(uint8_t port)
{

}
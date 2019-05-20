#include "system_reset.h"
#include "drivers/serial_usb.h"
#include <asf.h>

static bool _created = false;
static bool _reset_sources_enabled[SYSTEM_RESET_SOURCE__INVALID];

static uint8_t _usb_dtr_prev_value = false;

void _SystemHardReset(void)
{
	rstc_start_software_reset(RSTC);
}

void _UsbDtrChanged(uint8_t port, uint8_t set, void* arg)
{
	if(_reset_sources_enabled[SYSTEM_RESET_SOURCE__USB_DTR] && set == false && _usb_dtr_prev_value == true)
	{
		_SystemHardReset();
	}
	_usb_dtr_prev_value = set;
}

void SystemReset_Create(void)
{
	if(!_created)
	{
		memset(_reset_sources_enabled, 0, sizeof(_reset_sources_enabled));
		_created = true;
	}
}

void SystemReset_AddSource(system_reset_source_t source)
{
	if(_created)
	{
		_reset_sources_enabled[source] = true;

		if(source == SYSTEM_RESET_SOURCE__USB_DTR && SerialUsb_IsSetup())
		{
			SerialUsb_RegisterDTRCallback(_UsbDtrChanged, NULL);
		}
	}
}
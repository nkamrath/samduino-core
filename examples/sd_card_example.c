#include <asf.h>
#include "conf_board.h"
#include "drivers/serial_usb.h"
#include "utils/system_reset.h"
#include "drivers/sd_card.h"



int main(void)
{
	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	SerialUsb_Create();
	SystemReset_Create();
	SystemReset_AddSource(SYSTEM_RESET_SOURCE__USB_DTR);

	
	SdCard_Create();

	sd_file_t test_file = SdCard_OpenFile("test.txt", 9, FA_WRITE | FA_CREATE_ALWAYS);
	SdCard_Write(test_file, "testing a", 9);
	SdCard_CloseFile(test_file);

	return 0;
}
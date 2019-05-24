#include <asf.h>
#include "conf_board.h"
#include "drivers/serial_usb.h"
#include "utils/system_reset.h"
#include "os/thread_al.h"
#include "drivers/uart.h"
#include "drivers/spi.h"
#include "drivers/pin.h"
#include "drivers/mpu9250.h"
#include "os/queue_al.h"

#define TASK_MONITOR_STACK_SIZE            (2048/sizeof(portSTACK_TYPE))
#define TASK_MONITOR_STACK_PRIORITY        (tskIDLE_PRIORITY)
#define TASK_LED_STACK_SIZE                (1024/sizeof(portSTACK_TYPE))
#define TASK_LED_STACK_PRIORITY            (tskIDLE_PRIORITY)

uart_t uart1;
spi_t spi;
pin_t out_pin;
mpu9250_t mpu;
queue_t test_queue;

volatile int int_counter = 0;

bool out_pin_state = false;
volatile bool mpu_create_result = false;

char tx_data[5];

bool rx_data_buffer_ptr = false;
char rx_data[5];
char rx_data2[5];

char spi_tx_control[5];
char spi_tx_data[5];
char spi_rx_control[5];
char spi_rx_data[5];

static void task_monitor(void *pvParameters)
{
	UNUSED(pvParameters);
	int rx_ptr;
	for (;;) {
		if(Queue_Receive(test_queue, &rx_ptr, QUEUE_BLOCK_INFINITE))
		{
			char temp[20];
			int len = snprintf(temp, 64, "Got queue: %d\r\n", rx_ptr);
			SerialUsb_WriteBuffer(temp, len);
		}
	}
}

static void task_led(void *pvParameters)
{
	UNUSED(pvParameters);
	Uart_DmaWrite(uart1, tx_data, 4);
	int my_count = 0;
	for (;;) {
		vTaskDelay(1000);
		Queue_Send(test_queue, &my_count, QUEUE_BLOCK_NONE);
		my_count++;
	}
}

int main(void)
{
	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	SerialUsb_Create();
	SystemReset_Create();
	SystemReset_AddSource(SYSTEM_RESET_SOURCE__USB_DTR);
	tx_data[0] = 't';
	tx_data[1] = 'e';
	tx_data[2] = 's';
	tx_data[3] = 't';

	test_queue = Queue_Create(2, 4);

	/* create some threads */
	Thread_Create(task_monitor, "Monitor", TASK_MONITOR_STACK_SIZE, NULL, TASK_MONITOR_STACK_PRIORITY, NULL);
	Thread_Create(task_led, "Led", TASK_LED_STACK_SIZE, NULL, TASK_LED_STACK_PRIORITY, NULL);

	/* Start the scheduler. */
	vTaskStartScheduler();

	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}
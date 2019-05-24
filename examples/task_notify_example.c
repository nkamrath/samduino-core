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

pin_t in_pin;
pin_t out_pin;

thread_t task_monitor_thread;
thread_t led_thread;

static void task_monitor(void *pvParameters)
{
	UNUSED(pvParameters);
	for (;;) {
		Thread_WaitNotify(THREAD_BLOCK_INFINITE);
		SerialUsb_WriteBuffer("test\r\n", 6);
	}
}

static void task_led(void *pvParameters)
{
	UNUSED(pvParameters);
	for (;;) {
		vTaskDelay(1000);
		Pin_Set(out_pin);
		vTaskDelay(1000);
		Pin_Clear(out_pin);
	}
}

void pin_change(void* arg)
{
	Thread_Notify(task_monitor_thread);
}

int main(void)
{
	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	SerialUsb_Create();
	SystemReset_Create();
	SystemReset_AddSource(SYSTEM_RESET_SOURCE__USB_DTR);

	void* bank;
	uint32_t index;

	Pin_GetBankAndIndex(3, &bank, &index);
	pin_params_t in_pin_params = {
		.bank = bank,
		.bank_index = index,
		.direction = PIN_DIRECTION__INPUT,
		.initial_state = false,
		.pull_resistor = PIN_PULL_RESISTOR__DOWN,
		.interrupt_trigger = PIN_INTERRUPT_TRIGGER__RISING,
		.interrupt_callback = pin_change,
		.interrupt_callback_arg = NULL
	};
	in_pin = Pin_Create(&in_pin_params);


	Pin_GetBankAndIndex(4, &bank, &index);
	pin_params_t pin_params2 = {
		.bank = bank,
		.bank_index = index,
		.direction = PIN_DIRECTION__OUTPUT,
		.initial_state = true,
		.pull_resistor = PIN_PULL_RESISTOR__NONE,
	};
	out_pin = Pin_Create(&pin_params2);

	Pin_EnableInterrupt(in_pin);

	/* create some threads */
	Thread_Create(task_monitor, "Monitor", TASK_MONITOR_STACK_SIZE, NULL, TASK_MONITOR_STACK_PRIORITY, &task_monitor_thread);
	Thread_Create(task_led, "Led", TASK_LED_STACK_SIZE, NULL, TASK_LED_STACK_PRIORITY, NULL);

	/* Start the scheduler. */
	vTaskStartScheduler();

	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}
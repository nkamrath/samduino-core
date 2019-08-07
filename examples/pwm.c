#include <asf.h>
#include "conf_board.h"
#include "drivers/serial_usb.h"
#include "utils/system_reset.h"
#include "os/thread_al.h"
#include "drivers/pin.h"
#include "drivers/pwm.h"

#define TASK_MONITOR_STACK_SIZE            (2048/sizeof(portSTACK_TYPE))
#define TASK_MONITOR_STACK_PRIORITY        (tskIDLE_PRIORITY)
#define TASK_LED_STACK_SIZE                (1024/sizeof(portSTACK_TYPE))
#define TASK_LED_STACK_PRIORITY            (tskIDLE_PRIORITY)


thread_t task_monitor_thread;

static void task_monitor(void *pvParameters)
{
	UNUSED(pvParameters);

	for (;;) {
		Thread_WaitNotify(THREAD_BLOCK_INFINITE);
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

	pwm_params_t pwm_params = {
		.pwm_module = PWM,
		.pwm_channel = 2,
		.period = 250,
		.duty_cycle = 125,
		.gpio_controller = PIOA,
		.pin_index = PIO_PA2_IDX,
		.pin_mux_setting = PIO_PERIPH_A
	};
	pwm_t test_pwm = Pwm_Create(&pwm_params);


	/* create some threads */
	Thread_Create(task_monitor, "Monitor", TASK_MONITOR_STACK_SIZE, NULL, TASK_MONITOR_STACK_PRIORITY, &task_monitor_thread);

	/* Start the scheduler. */
	vTaskStartScheduler();

	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}
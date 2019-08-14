#include <asf.h>
#include "conf_board.h"
#include "drivers/serial_usb.h"
#include "utils/system_reset.h"
#include "os/thread_al.h"
#include "drivers/pin.h"
#include "drivers/pwm.h"
#include "drivers/usart.h"
#include "drivers/timer.h"

#define TASK_MONITOR_STACK_SIZE            (2048/sizeof(portSTACK_TYPE))
#define TASK_MONITOR_STACK_PRIORITY        (tskIDLE_PRIORITY)
#define TASK_LED_STACK_SIZE                (1024/sizeof(portSTACK_TYPE))
#define TASK_LED_STACK_PRIORITY            (tskIDLE_PRIORITY)


thread_t task_monitor_thread;
timer_counter_t timer_pwm;

static void task_monitor(void *pvParameters)
{
	UNUSED(pvParameters);

	for (;;) {
		Thread_Delay(1000);
		// Timer_SetDutyCycle(timer_pwm, 0, 10);
		// Timer_SetDutyCycle(timer_pwm, 1, 20);
	}
}

volatile char buffer[4] = {'t', 'e', 's', 't' };

void tx_ready_callback(void* arg, void** next_tx_buffer, uint32_t* next_tx_buffer_length)
{
	*next_tx_buffer = buffer;
	*next_tx_buffer_length = 4;
}
void rx_ready_callback(void* arg, void* rx_data, uint32_t rx_length, void** next_rx_buffer, uint32_t* next_rx_buffer_length)
{
	*next_rx_buffer = rx_data;
	*next_rx_buffer_length = rx_length;
}

static void timer_callback(void* arg)
{
	Timer_SetDutyCycle(timer_pwm, 0, 30);
}

int main(void)
{
	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	SerialUsb_Create();
	SystemReset_Create();
	SystemReset_AddSource(SYSTEM_RESET_SOURCE__USB_DTR);

	//these pwms are setup on digitial only poins
	//PA0 = 4 for timer 0, TIOA0
	//PA1 = 5 for timer 0, TIOB0
	//PA2 = 6 for PWM 0 channel 0
	//PA24 = 31 for PWM 0 channel 1
	//pA25 = 32 for pwm 0 channel 2
	//PA7 = 34 for pwm 0 channel 3


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

	pwm_params_t pwm_params2 = {
		.pwm_module = PWM,
		.pwm_channel = 1,
		.period = 250,
		.duty_cycle = 50,
		.gpio_controller = PIOA,
		.pin_index = PIO_PA24_IDX,
		.pin_mux_setting = PIO_PERIPH_B
	};
	pwm_t test_pwm2 = Pwm_Create(&pwm_params2);

	pwm_params_t pwm_params3 = {
		.pwm_module = PWM,
		.pwm_channel = 2,
		.period = 250,
		.duty_cycle = 70,
		.gpio_controller = PIOA,
		.pin_index = PIO_PA25_IDX,
		.pin_mux_setting = PIO_PERIPH_B
	};
	pwm_t test_pwm3 = Pwm_Create(&pwm_params3);

	pwm_params_t pwm_params4 = {
		.pwm_module = PWM,
		.pwm_channel = 3,
		.period = 250,
		.duty_cycle = 210,
		.gpio_controller = PIOA,
		.pin_index = PIO_PA7_IDX,
		.pin_mux_setting = PIO_PERIPH_B
	};
	pwm_t test_pwm4 = Pwm_Create(&pwm_params4);

	usart_params_t usart_params = {
		.dev_ptr = USART0,
		.baud_rate = 9600,
		.callback_arg = NULL,
		.parity = US_MR_PAR_NO,
		.stop_bits = US_MR_NBSTOP_1_BIT,
		.tx_ready_callback = tx_ready_callback,
		.rx_ready_callback = rx_ready_callback
	};

	timer_counter_params_t tc_params = {
		.mode = TIMER_COUNTER_MODE_PWM,
		.timer_module = TC0,
		.channel = 0,
		.period_us = 250,
		.callback_function = NULL,
		.pwm_duty_cycle1 = 100,
		.pwm_duty_cycle2 = 200,
		.pwm_gpio1_controller = PIOA,
		.pwm_gpio1_pin_index = PIO_PA0_IDX,
		.pwm_gpio1_pin_mux_setting = PIO_PERIPH_B,
		.pwm_gpio2_controller = PIOA,
		.pwm_gpio2_pin_index = PIO_PA1_IDX,
		.pwm_gpio2_pin_mux_setting = PIO_PERIPH_B
	};

	timer_pwm = Timer_Create(&tc_params);

	Timer_Start(timer_pwm);

	//this is an example of how to run a callback function on a timer interrupt callback
	timer_counter_params_t tc_params2 = {
		.mode = TIMER_COUNTER_MODE_TIMER,
		.timer_module = TC0,
		.channel = 2,
		.period_us = 1000,
		.callback_function = timer_callback
	};

	timer_counter_t timer_counter = Timer_Create(&tc_params2);

	Timer_Start(timer_pwm);
	Timer_Start(timer_counter);

	// void* bank = NULL;
	// uint32_t bank_index = 0;

	// Pin_GetBankAndIndex(12, &bank, &bank_index);

	// pin_params_t pin_params = {
	// 	.bank = bank,
	// 	.bank_index = bank_index,
	// 	.direction = PIN_DIRECTION__OUTPUT,
	// 	.initial_state = true,

	// };
	// pin_t test_pin = Pin_Create(&pin_params);
	
	// Pin_Set(test_pin);
	// Pin_Clear(test_pin);
	// Pin_Set(test_pin);
	// Pin_Clear(test_pin);
	// Pin_Set(test_pin);
	// Pin_Clear(test_pin);
	// Pin_Set(test_pin);
	// Pin_Clear(test_pin);
	// Pin_Set(test_pin);
	// Pin_Clear(test_pin);

	usart_t usart0 = Usart_Create(&usart_params);
	buffer[3] = 'n';
	Usart_DmaWrite(usart0, buffer, 4);

	/* create some threads */
	Thread_Create(task_monitor, "Monitor", TASK_MONITOR_STACK_SIZE, NULL, TASK_MONITOR_STACK_PRIORITY, &task_monitor_thread);

	/* Start the scheduler. */
	vTaskStartScheduler();

	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}
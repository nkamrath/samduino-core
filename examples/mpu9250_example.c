#include <asf.h>
#include "conf_board.h"
#include "drivers/serial_usb.h"
#include "utils/system_reset.h"
#include "os/thread_al.h"
#include "drivers/uart.h"
#include "drivers/spi.h"
#include "drivers/pin.h"
#include "drivers/mpu9250.h"

#define TASK_MONITOR_STACK_SIZE            (2048/sizeof(portSTACK_TYPE))
#define TASK_MONITOR_STACK_PRIORITY        (tskIDLE_PRIORITY)
#define TASK_LED_STACK_SIZE                (1024/sizeof(portSTACK_TYPE))
#define TASK_LED_STACK_PRIORITY            (tskIDLE_PRIORITY)

uart_t uart1;
spi_t spi;
pin_t out_pin;
mpu9250_t mpu;
thread_t task_monitor_thread;

volatile int int_counter = 0;

bool out_pin_state = false;
volatile bool mpu_create_result = false;

volatile int16_t gyro[3];
volatile int16_t accel[3];
volatile int32_t quat[4];
volatile int16_t mag[3];
volatile bool quat_ready;
volatile bool mag_ready;

volatile int mag_data_read = 0;
volatile int mag_data_written = 0;

static void task_monitor(void *pvParameters)
{
	UNUSED(pvParameters);

	for (;;) {
		Thread_WaitNotify(THREAD_BLOCK_INFINITE);
	}
}

static void pin_change(void* arg)
{
	int_counter++;
	Mpu9250_GetInterruptStatus(mpu);
	Mpu9250_ReadGyro(mpu, gyro);
	Mpu9250_ReadAccel(mpu, accel);
	quat_ready = Mpu9250_ReadQuat(mpu, quat);

	if(int_counter == 25)
	{
		mag_ready = Mpu9250_ReadMag(mpu, mag);
		int_counter = 0;
	}

	
	if(quat_ready)
	{
		uint32_t marker = 0xa5a5;
		SerialUsb_WriteBuffer(&marker, 4);
		SerialUsb_WriteBuffer(quat, 16);
		SerialUsb_WriteBuffer(accel, 6);
		SerialUsb_WriteBuffer(gyro, 6);
		SerialUsb_WriteBuffer(mag, 6);
	}

	//Thread_Notify(task_monitor_thread);
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

	void* int_bank;
	uint32_t int_index;

	Pin_GetBankAndIndex(2, &bank, &index);
	Pin_GetBankAndIndex(3, &int_bank, &int_index);

	mpu9250_params_t mpu_params = {
		.spi_params = {
			.dev_ptr = SPI,
			.clk_hz = 2000000,
			.polarity = 0,
			.phase = 0,
			.transfer_complete_callback = NULL
		},
		.cs_pin_params = {
			.bank = bank,
			.bank_index = index,
			.direction = PIN_DIRECTION__OUTPUT,
			.initial_state = true,
			.pull_resistor = PIN_PULL_RESISTOR__UP
		},
		.int_pin_params = {
			.bank = int_bank,
			.bank_index = int_index,
			.direction = PIN_DIRECTION__INPUT,
			.initial_state = false,
			.pull_resistor = PIN_PULL_RESISTOR__DOWN,
			.interrupt_trigger = PIN_INTERRUPT_TRIGGER__RISING,
			.interrupt_callback = pin_change,
			.interrupt_callback_arg = NULL
		},
		.gyro_scale = GYRO_SCALE__2000DPS,
		.accel_scale = ACCEL_SCALE__4G
	};

	mpu = Mpu9250_Create(&mpu_params);
	if(mpu)
	{
		mpu_create_result = true;
		Mpu9250_EnableInterrupts(mpu);
		Mpu9250_EnableDmp(mpu);
	}


	/* create some threads */
	Thread_Create(task_monitor, "Monitor", TASK_MONITOR_STACK_SIZE, NULL, TASK_MONITOR_STACK_PRIORITY, &task_monitor_thread);

	/* Start the scheduler. */
	vTaskStartScheduler();

	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}
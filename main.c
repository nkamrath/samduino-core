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

	for (;;) {
		vTaskDelay(1000);
	}
}

static void task_led(void *pvParameters)
{
	UNUSED(pvParameters);
	Uart_DmaWrite(uart1, tx_data, 4);
	for (;;) {
		vTaskDelay(1000);
		Spi_DmaTransfer(spi, spi_tx_control, spi_tx_data, NULL, NULL, 4, 4);
		if(out_pin_state == false)
		{
			Pin_Set(out_pin);
		}
		else
		{
			Pin_Clear(out_pin);
		}
		out_pin_state = !out_pin_state;

		if(mpu_create_result)
		{
			//SerialUsb_WriteBuffer("s\n", 2);
		}
		else
		{
			//SerialUsb_WriteBuffer("f\n", 2);
		}

		char tmp[20];
		snprintf(tmp, 20, "irqs: %d\r\n", int_counter);
		//SerialUsb_WriteBuffer(tmp, 20);
	}
}

static void spi_transfer_complete(spi_t port)
{
	//Spi_DmaTransfer(spi, spi_tx_control, spi_tx_data, spi_rx_control, spi_rx_data, 4, 4);
	Spi_DmaTransfer(spi, spi_tx_control, spi_tx_data, NULL, NULL, 4, 4);
}

static void uart_rx_ready(uart_t port)
{
	uint8_t readData;
	uart_read(UART0, &readData);
	
	if(rx_data_buffer_ptr)
	{
		//SerialUsb_WriteBuffer(rx_data2, 4);
		rx_data2[0] = '0';
		rx_data2[1] = '0';
		rx_data2[2] = '0';
		rx_data2[3] = '0';
		Uart_DmaRead(uart1, rx_data, 4);
	}
	else
	{
		//SerialUsb_WriteBuffer(rx_data, 4);
		rx_data[0] = '0';
		rx_data[1] = '0';
		rx_data[2] = '0';
		rx_data[3] = '0';
		Uart_DmaRead(uart1, rx_data2, 4);
	}
	rx_data_buffer_ptr = !rx_data_buffer_ptr;
	//SerialUsb_WriteBuffer(" uart rx rdy\r\n", 14);
}

static void uart_tx_ready(uart_t port)
{
	Uart_DmaWrite(uart1, tx_data, 4);
}

static void pin_change(void* arg)
{

	Mpu9250_GetInterruptStatus(mpu);
	int16_t gyro[3];
	Mpu9250_ReadGyro(mpu, gyro);
	int16_t accel[3];
	Mpu9250_ReadAccel(mpu, accel);

	int32_t quat[4];
	bool quat_ready = Mpu9250_ReadQuat(mpu, quat);

	if(quat_ready)
	{
		int_counter++;
		if(int_counter == 5)
		{
			uint32_t marker = 0xa5a5;
			SerialUsb_WriteBuffer(&marker, 4);
			SerialUsb_WriteBuffer(quat, 16);
			SerialUsb_WriteBuffer(accel, 6);
			SerialUsb_WriteBuffer(gyro, 6);
			int_counter = 0;
		}
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

	uart_params_t uart_params = {
		.dev_ptr = UART0,
		.baud_rate = 115200,
		.tx_ready_callback = uart_tx_ready,
		.rx_ready_callback = uart_rx_ready
	};
	uart1 = Uart_Create(&uart_params);
	Uart_DmaRead(uart1, rx_data, 4);

	// spi_params_t spi_params = {
	// 	.dev_ptr = SPI,
	// 	.clk_hz = 1000000,
	// 	.polarity = 0,
	// 	.phase = 0,
	// 	.transfer_complete_callback = NULL//spi_transfer_complete
	// };
	// spi = Spi_Create(&spi_params);

	// spi_tx_control[0] = 0x00;
	// spi_tx_control[1] = 0xf0;
	// spi_tx_control[2] = 0x00;
	// spi_tx_control[3] = 0x0f;

	// spi_tx_data[0] = 0x01;
	// spi_tx_data[1] = 0x02;
	// spi_tx_data[2] = 0x03;
	// spi_tx_data[3] = 0x04;
	//Spi_DmaTransfer(spi, spi_tx_control, spi_tx_data, spi_rx_control, spi_rx_data, 4, 4);

	void* bank;
	uint32_t index;

	void* int_bank;
	uint32_t int_index;

	Pin_GetBankAndIndex(2, &bank, &index);
	Pin_GetBankAndIndex(3, &int_bank, &int_index);

	mpu9250_params_t mpu_params = {
		.spi_params = {
			.dev_ptr = SPI,
			.clk_hz = 1000000,
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

	// pin_t pin = Pin_Create(&pin_params);

	// Pin_GetBankAndIndex(5, &bank, &index);


	// .bank = bank,
	// 		.bank_index = index,
	// 		.direction = PIN_DIRECTION__INPUT,
	// 		.initial_state = false,
	// 		.pull_resistor = PIN_PULL_RESISTOR__DOWN,
	// 		.interrupt_trigger = PIN_INTERRUPT_TRIGGER__CHANGE,
	// 		.interrupt_callback = pin_change,
	// 		.interrupt_callback_arg = NULL

	// pin_params_t pin_params2 = {
	// 	.bank = bank,
	// 	.bank_index = index,
	// 	.direction = PIN_DIRECTION__OUTPUT,
	// 	.initial_state = true,
	// 	.pull_resistor = PIN_PULL_RESISTOR__NONE,
	// };

	// out_pin = Pin_Create(&pin_params2);


	/* create some threads */
	Thread_Create(task_monitor, "Monitor", TASK_MONITOR_STACK_SIZE, NULL, TASK_MONITOR_STACK_PRIORITY);
	Thread_Create(task_led, "Led", TASK_LED_STACK_SIZE, NULL, TASK_LED_STACK_PRIORITY);

	/* Start the scheduler. */
	vTaskStartScheduler();

	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}
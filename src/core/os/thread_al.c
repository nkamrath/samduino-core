#include "thread_al.h"
#include "utils/cpu_context.h"


void Thread_Create(void(*thread_function)(void* arg), char* thread_name, uint32_t thread_stack_size,
						void* arg, uint8_t thread_priority, thread_t* thread_handle)
{
	xTaskCreate(thread_function, thread_name, thread_stack_size, arg, thread_priority, thread_handle);
}

void Thread_Delay(uint32_t time_ms)
{
	vTaskDelay(time_ms / portTICK_PERIOD_MS);
}

void Thread_WaitNotify(uint32_t timeout)
{
	uint32_t notified_value;
	xTaskNotifyWait(0x00, 0xffffffff, &notified_value, timeout);
}

void Thread_Notify(thread_t thread)
{
	uint32_t flags = 0;
	uint32_t action = 0;
	if(CpuContext_Get() == CPU_CONTEXT__THREAD)
	{
		xTaskNotify(thread, flags, action);
	}
	else
	{
		BaseType_t woken = 0;
		xTaskNotifyFromISR(thread, flags, action, &woken);
	}
}
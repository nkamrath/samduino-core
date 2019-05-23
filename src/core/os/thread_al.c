#include "thread_al.h"

thread_t Thread_Create(void(*thread_function)(void* arg), char* thread_name, uint32_t thread_stack_size,
						void* arg, uint8_t thread_priority)
{
	TaskHandle_t * const pxCreatedTask = NULL;
	xTaskCreate(thread_function, thread_name, thread_stack_size, arg, thread_priority, pxCreatedTask);
	return pxCreatedTask;
}
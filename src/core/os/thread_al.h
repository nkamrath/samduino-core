#ifndef _OS_THREAD_AL_H_
#define _OS_THREAD_AL_H_

#include <asf.h>

#define THREAD_BLOCK_NONE 0
#define THREAD_BLOCK_INFINITE portMAX_DELAY

typedef TaskHandle_t thread_t;
void Thread_Create(void(*thread_function)(void* arg), char* thread_name, uint32_t thread_stack_size,
						void* arg, uint8_t thread_priority, thread_t* thread_handle);

void Thread_Delay(uint32_t time_ms);

void Thread_WaitNotify(uint32_t timeout);
void Thread_Notify(thread_t thread);

#endif
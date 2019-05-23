#ifndef _OS_THREAD_AL_H_
#define _OS_THREAD_AL_H_

#include <asf.h>

typedef void* thread_t;
thread_t Thread_Create(void(*thread_function)(void* arg), char* thread_name, uint32_t thread_stack_size,
						void* arg, uint8_t thread_priority);

#endif
#include "queue_al.h"
#include "utils/cpu_context.h"

queue_t Queue_Create(uint32_t queue_length, uint32_t item_size)
{
	queue_t* new_queue = xQueueCreate(queue_length, item_size);
	return new_queue;
}

bool Queue_Send(queue_t queue, void* item, uint32_t timeout)
{
	if(CpuContext_Get() == CPU_CONTEXT__THREAD)
	{
		if(xQueueSend(queue, item, timeout) == pdTRUE)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		BaseType_t higher_task_woken = 0;
		if(xQueueSendFromISR(queue, item, &higher_task_woken) == pdTRUE)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

bool Queue_Receive(queue_t queue, void* item, uint32_t timeout)
{
	if(CpuContext_Get() == CPU_CONTEXT__THREAD)
	{
		if(xQueueReceive(queue, item, timeout) == pdTRUE)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		BaseType_t higher_task_woken = 0;
		if(xQueueReceiveFromISR(queue, item, &higher_task_woken) == pdTRUE)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}
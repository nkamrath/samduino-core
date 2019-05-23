#ifndef _OS_QUEUE_H_
#define _OS_QUEUE_H_

#include <asf.h>

typedef void* queue_t;

#define QUEUE_BLOCK_NONE 0
#define QUEUE_BLOCK_INFINTE portMAX_DELAY

queue_t Queue_Create(uint32_t queue_length, uint32_t item_size);

bool Queue_Send(queue_t queue, void* item, uint32_t timeout);
bool Queue_Receive(queue_t queue, void* item, uint32_t timeout);

#endif
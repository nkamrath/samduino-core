#ifndef _DRIVERS_PIN_H_
#define _DRIVERS_PIN_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
	PIN_DIRECTION__INPUT,
	PIN_DIRECTION__OUTPUT
} pin_dir_t;

typedef enum
{
	PIN_PULL_RESISTOR__NONE,
	PIN_PULL_RESISTOR__DOWN,
	PIN_PULL_RESISTOR__UP
} pin_pull_resistor_t;

typedef enum
{
	PIN_INTERRUPT_TRIGGER__FALLING,
	PIN_INTERRUPT_TRIGGER__RISING,
	PIN_INTERRUPT_TRIGGER__CHANGE,
	PIN_INTERRUPT_TRIGGER__HIGH,
	PIN_INTERRUPT_TRIGGER__LOW
} pin_interrupt_trigger_t;

typedef struct
{
	void* bank;
	uint32_t bank_index;
	pin_dir_t direction;
	bool initial_state;
	pin_pull_resistor_t pull_resistor;
	pin_interrupt_trigger_t interrupt_trigger;
	void (*interrupt_callback)(void* arg);
	void* interrupt_callback_arg;
} pin_params_t;

typedef void* pin_t;

pin_t Pin_Create(pin_params_t* params);

void Pin_Set(pin_t pin);
void Pin_Clear(pin_t pin);

void Pin_EnableInterrupt(pin_t pin);
void Pin_DisableInterrupt(pin_t pin);
bool Pin_Read(pin_t pin);

bool Pin_GetBankAndIndex(uint8_t number, void** bank, uint32_t* bank_index);

#endif
#include "pin.h"
#include <asf.h>

#define PIOA_FIRST_PIN			0
#define PIOA_NUM_PINS			10
#define PIOB_FIRST_PIN			(PIOA_FIRST_PIN + PIOA_NUM_PINS)
#define PIOB_NUM_PINS			10

#define NUMBER_PINS	35
const uint8_t PINS[NUMBER_PINS] = {
	PIO_PB13_IDX, PIO_PB14_IDX, PIO_PB7_IDX, PIO_PB6_IDX , PIO_PA0_IDX, PIO_PA1_IDX, PIO_PA2_IDX,
	PIO_PB4_IDX, PIO_PB5_IDX, PIO_PA3_IDX, PIO_PA4_IDX, PIO_PA5_IDX, PIO_PA6_IDX, PIO_PA9_IDX,
	PIO_PA10_IDX, PIO_PA14_IDX, PIO_PA12_IDX, PIO_PA13_IDX, PIO_PA17_IDX, PIO_PA18_IDX, PIO_PA19_IDX,
	PIO_PA20_IDX, PIO_PB0_IDX, PIO_PB1_IDX, PIO_PB2_IDX, PIO_PB3_IDX, PIO_PA21_IDX, PIO_PA22_IDX,
	PIO_PA23_IDX, PIO_PA16_IDX, PIO_PA15_IDX, PIO_PA24_IDX, PIO_PA25_IDX, PIO_PA8_IDX, PIO_PA7_IDX
};

#define A0 PIO_PA17_IDX
#define A1 PIO_PA18_IDX
#define A2 PIO_PA19_IDX
#define A3 PIO_PA20_IDX
#define A4 PIO_PB0_IDX
#define A5 PIO_PB1_IDX
#define A6 PIO_PB2_IDX
#define A7 PIO_PB3_IDX
#define A8 PIO_PA21_IDX
#define A9 PIO_PA22_IDX

const uint8_t ADC_PINS[] = {
	A0,
	A1,
	A2,
	A3,
	A4,
	A5,
	A6,
	A7,
	A8,
	A9
};

// const adc_channel_num_t ADC_CHANNEL_PINS[] = {
// 	ADC_CHANNEL_0,
// 	ADC_CHANNEL_1,
// 	ADC_CHANNEL_2,
// 	ADC_CHANNEL_3,
// 	ADC_CHANNEL_4,
// 	ADC_CHANNEL_5,
// 	ADC_CHANNEL_6,
// 	ADC_CHANNEL_7,
// 	ADC_CHANNEL_8,
// 	ADC_CHANNEL_9
// };

#define NUMBER_ANALOG_PINS 10
const uint8_t ANALOG_PINS[NUMBER_ANALOG_PINS] = {
	A0, A1, A2, A3, A4, A5, A6, A7, A8, A9 
};
								   
#define GET_PIN_INDEX(pin) (PINS[pin])// % 32)
#define GET_PIN_SHIFT(pin) (PINS[pin] % 32)

typedef struct
{
	void* bank;
	uint32_t mask;
	uint32_t bank_index;
	void (*interrupt_callback)(void* arg);
	void* interrupt_callback_arg;
} _pin_t;

static _pin_t _pin_bank_a[32];
static _pin_t _pin_bank_b[32];

void PIOA_Handler()
{
	uint32_t status = pio_get_interrupt_status(PIOA);
	status &= pio_get_interrupt_mask(PIOA);
	for(int i = 0; i < 32; i++)
	{
		if((status >> i) & 0x01 && _pin_bank_a[i].interrupt_callback)
		{
			_pin_bank_a[i].interrupt_callback(_pin_bank_a[i].interrupt_callback_arg);
		}
	}
}

void PIOB_Handler()
{
	uint32_t status = pio_get_interrupt_status(PIOB);
	status &= pio_get_interrupt_mask(PIOB);
	for(int i = 0; i < 32; i++)
	{
		if((status >> i) & 0x01 && _pin_bank_b[i].interrupt_callback)
		{
			_pin_bank_b[i].interrupt_callback(_pin_bank_b[i].interrupt_callback_arg);
		}
	}
}

Pio* _get_pio_ptr(uint8_t pin)
{
	if(pin < PIO_PB0_IDX)
	{
		return PIOA;
	}
	else
	{
		return PIOB;
	}
}

pin_t Pin_Create(pin_params_t* params)
{
	_pin_t* pin = NULL;
	if(params->bank == PIOA)
	{
		pin = &_pin_bank_a[params->bank_index];
	}
	else if(params->bank == PIOB)
	{
		pin = &_pin_bank_b[params->bank_index];
	}
	else
	{
		return NULL;
	}

	pin->bank = params->bank;
	pin->bank_index = params->bank_index;
	pin->mask = (1 << params->bank_index);

	if(params->direction == PIN_DIRECTION__INPUT)
	{
		pio_set_input(pin->bank, pin->mask, 0);
	}
	else
	{
		pio_set_output(pin->bank, pin->mask, params->initial_state, DISABLE, DISABLE);
	}

	if(params->pull_resistor == PIN_PULL_RESISTOR__DOWN)
	{
		pio_pull_up(pin->bank, pin->mask, false);
		pio_pull_down(pin->bank, pin->mask, true);
	}
	else if(params->pull_resistor == PIN_PULL_RESISTOR__UP)
	{
		pio_pull_down(pin->bank, pin->mask, false);
		pio_pull_up(pin->bank, pin->mask, true);
	}
	else
	{
		pio_pull_down(pin->bank, pin->mask, false);
		pio_pull_up(pin->bank, pin->mask, false);
	}

	if(params->direction == PIN_DIRECTION__INPUT && params->interrupt_callback)
	{
		pin->interrupt_callback = params->interrupt_callback;
		pin->interrupt_callback_arg = params->interrupt_callback_arg;

		uint32_t interrupt_attributes = PIO_IT_AIME;
		//change the mode into PIO correct mask
		if(params->interrupt_trigger == PIN_INTERRUPT_TRIGGER__RISING)
		{
			interrupt_attributes |= PIO_IT_RISE_EDGE;
		}
		else if(params->interrupt_trigger == PIN_INTERRUPT_TRIGGER__FALLING)
		{
			interrupt_attributes |= PIO_IT_FALL_EDGE;
		}
		else if(params->interrupt_trigger == PIN_INTERRUPT_TRIGGER__LOW)
		{
			interrupt_attributes |= PIO_IT_LOW_LEVEL;
		}
		else if(params->interrupt_trigger == PIN_INTERRUPT_TRIGGER__HIGH)
		{
			interrupt_attributes |= PIO_IT_HIGH_LEVEL;
		}
		else if(params->interrupt_trigger == PIN_INTERRUPT_TRIGGER__CHANGE)
		{
			interrupt_attributes |= PIO_IT_EDGE;
		}

		pio_enable_interrupt(pin->bank, pin->mask);
		pio_configure_interrupt(pin->bank, pin->mask, interrupt_attributes);
	}

	return pin;
}

void Pin_Set(pin_t pin)
{
	_pin_t* temp = (_pin_t*)pin;
	pio_set(temp->bank, temp->mask);
}

void Pin_Clear(pin_t pin)
{
	_pin_t* temp = (_pin_t*)pin;
	pio_clear(temp->bank, temp->mask);
}

void Pin_EnableInterrupt(pin_t pin)
{

}

void Pin_DisableInterrupt(pin_t pin)
{

}

bool Pin_Read(pin_t pin)
{
	return true;
}

bool Pin_GetBankAndIndex(uint8_t number, void** bank, uint32_t* index)
{
	*bank = _get_pio_ptr(GET_PIN_INDEX(number));
	*index = GET_PIN_SHIFT(number);
	return true;
}
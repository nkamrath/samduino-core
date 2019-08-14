#ifndef _DRIVERS_TIMER_H_
#define _DRIVERS_TIMER_H_

#include <asf.h>
#include <stdint.h>

typedef void* timer_counter_t;

typedef enum
{
	TIMER_COUNTER_MODE_TIMER,
	TIMER_COUNTER_MODE_PWM
} timer_counter_mode_t;

typedef struct
{
	timer_counter_mode_t mode;
	void* timer_module;
	uint8_t channel;
	uint32_t period_us;
	void (*callback_function)(void* arg);
	void* callback_arg;
	uint32_t pwm_duty_cycle1;
	uint32_t pwm_duty_cycle2;
	void* pwm_gpio1_controller;
	uint32_t pwm_gpio1_pin_index;
	uint32_t pwm_gpio1_pin_mux_setting;
	void* pwm_gpio2_controller;
	uint32_t pwm_gpio2_pin_index;
	uint32_t pwm_gpio2_pin_mux_setting;
} timer_counter_params_t;

timer_counter_t Timer_Create(timer_counter_params_t* options);
bool Timer_Start(timer_counter_t timer);
bool Timer_SetPeriod(timer_counter_t timer, uint32_t period_us);
bool Timer_SetDutyCycle(timer_counter_t timer, uint8_t channel, uint32_t duty_cycle_us);

#endif
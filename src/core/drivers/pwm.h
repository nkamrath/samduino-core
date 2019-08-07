#ifndef _DRIVERS_PWM_H_
#define _DRIVERS_PWM_H_

#include <asf.h>
#include <stdint.h>

typedef void* pwm_t;

typedef struct
{
	void* gpio_controller;
	uint32_t pin_number;
	uint32_t pin_mux_setting;
} pwm_output_pin_config_t;

typedef struct
{
	void* pwm_module;
	uint8_t pwm_channel;
	uint32_t period;
	uint32_t duty_cycle;
	pwm_output_pin_config_t* output_pin_configs;
} pwm_params_t;

pwm_t Pwm_Create(pwm_params_t* params);
void Pwm_ChangeDuty(pwm_t pwm, uint32_t new_duty);
void Pwm_ChangePeriod(pwm_t pwm, uint32_t new_period);

#endif
#include "pwm.h"

typedef struct
{
	void* pwm_module;
	pwm_channel_t channel;
} _pwm_t;

typedef struct
{
	uint32_t clk_a;
	uint32_t clk_b;
} _pwm_module_settings_t;

_pwm_module_settings_t _pwm_module_settings;

_pwm_t _pwm_channels[4];

pwm_t Pwm_Create(pwm_params_t* params)
{
	_pwm_t* handle = NULL;
	if(params->pwm_module == PWM)
	{
		handle = &_pwm_channels[params->pwm_channel];
		handle->pwm_module = params->pwm_module;

		pmc_enable_periph_clk(ID_PWM);
		pwm_channel_disable(handle->pwm_module, handle->channel.channel);

		if(_pwm_module_settings.clk_a == 0)
		{
			//setup the frequency of clk a
			_pwm_module_settings.clk_a = 1000 * 100;

			pwm_clock_t clock_settings = {
				.ul_clka = _pwm_module_settings.clk_a,
				.ul_clkb = 0,
				.ul_mck = sysclk_get_peripheral_hz()
			};
			pwm_init(PWM, &clock_settings);

		}
		// else
		// {
		// 	handle->channel.ul_prescaler = PWM_CMR_CPRE_CLKB;
		// }

		handle->channel.ul_prescaler = PWM_CMR_CPRE_CLKA;

		//TODO: perform pin output setup if there are pin configs included in the pwm config
		pio_set_peripheral(params->gpio_controller, params->pin_mux_setting, (1<<params->pin_index));
		
		handle->channel.ul_period = params->period;
		handle->channel.ul_duty = params->duty_cycle;
		handle->channel.channel = params->pwm_channel;
		handle->channel.polarity = PWM_HIGH;
		pwm_channel_init(PWM, &handle->channel);
	}

	return handle;
}

void Pwm_Enable(pwm_t pwm)
{
	_pwm_t* handle = (_pwm_t*)pwm;
	pwm_channel_enable(handle->pwm_module, handle->channel.channel);
}

void Pwm_Disable(pwm_t pwm)
{
	_pwm_t* handle = (_pwm_t*)pwm;
	pwm_channel_disable(handle->pwm_module, handle->channel.channel);	
}

void Pwm_ChangeDuty(pwm_t pwm, uint32_t new_duty)
{
	_pwm_t* handle = (_pwm_t*)pwm;
	pwm_channel_update_duty(handle->pwm_module, &handle->channel, new_duty);
}

void Pwm_ChangePeriod(pwm_t pwm, uint32_t new_period)
{
	_pwm_t* handle = (_pwm_t*)pwm;
	pwm_channel_update_period(handle->pwm_module, &handle->channel, new_period);
}
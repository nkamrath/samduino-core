#include "pwm.h"

typedef struct
{
	void* pwm_module;
	uint8_t channel;
	uint32_t period;
	uint32_t duty_cycle;
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
		handle->period = params->period;
		handle->duty_cycle = params->duty_cycle;

		pmc_enable_periph_clk(ID_PWM);
		pwm_channel_disable(handle->pwm_module, handle->channel);

		pwm_channel_t temp;

		if(params->period != (_pwm_module_settings.clk_a * 1000) && _pwm_module_settings.clk_a == 0)
		{
			//setup the frequency of clk a
			_pwm_module_settings.clk_a = 1000 * handle->period;

			pwm_clock_t clock_settings = {
				.ul_clka = _pwm_module_settings.clk_a,
				.ul_clkb = 0,
				.ul_mck = sysclk_get_peripheral_hz()
			};
			pwm_init(PWM, &clock_settings);

			temp.ul_prescaler = PWM_CMR_CPRE_CLKA;

		}
		else
		{
			temp.ul_prescaler = PWM_CMR_CPRE_CLKB;
		}

		//TODO: perform pin output setup if there are pin configs included in the pwm config

		
		temp.ul_period = handle->period;
		temp.ul_duty = handle->duty_cycle;
		temp.channel = handle->channel;
		pwm_channel_init(PWM, &temp);

	}

	return handle;
}
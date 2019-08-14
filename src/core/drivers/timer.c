#include "timer.h"

typedef struct
{
	timer_counter_mode_t mode;
	void* timer_module;
	uint8_t channel;
	uint32_t period;
	uint32_t duty_cycle1;
	uint32_t duty_cycle2;

	void (*callback_function)(void* arg);
	void* callback_arg;

	void* pwm_gpio1_controller;
	uint32_t pwm_gpio1_pin_index;
	void* pwm_gpio2_controller;
	uint32_t pwm_gpio2_pin_index;
} _timer_counter_t;

static bool _enabled = false;
_timer_counter_t _timers[3];

void TC0_Handler()
{
	uint8_t channel = 0;
	uint32_t status = tc_get_status(TC0, channel);
	
	//if the interrupt is on this
	if((status & TC_SR_CPCS) && _timers[channel].callback_function)
	{
		_timers[channel].callback_function(_timers[channel].callback_arg);
	}
}

void TC1_Handler()
{
	uint8_t channel = 1;
	uint32_t status = tc_get_status(TC0, channel);
	
	//if the interrupt is on this
	if((status & TC_SR_CPCS) && _timers[channel].callback_function)
	{
		_timers[channel].callback_function(_timers[channel].callback_arg);
	}
}

void TC2_Handler()
{
	uint8_t channel = 2;
	uint32_t status = tc_get_status(TC0, channel);
	
	//if the interrupt is on this
	if((status & TC_SR_CPCS) && _timers[channel].callback_function)
	{
		_timers[channel].callback_function(_timers[channel].callback_arg);
	}
}

timer_counter_t Timer_Create(timer_counter_params_t* options)
{
	uint32_t sysclk = sysclk_get_cpu_hz();

	if(!_enabled)
	{
		pmc_enable_periph_clk(ID_TC0);
		pmc_enable_periph_clk(ID_TC1);
		pmc_enable_periph_clk(ID_TC2);
		_enabled = true;
	}

	_timer_counter_t* handle = &_timers[options->channel];
	handle->mode = options->mode;
	handle->timer_module = options->timer_module;
	handle->channel = options->channel;
	handle->period = options->period_us;
	handle->duty_cycle1 = options->pwm_duty_cycle1;
	handle->duty_cycle2 = options->pwm_duty_cycle2;
	handle->callback_function = options->callback_function;
	handle->callback_arg = options->callback_arg;
	handle->pwm_gpio1_controller = options->pwm_gpio1_controller;
	handle->pwm_gpio1_pin_index = options->pwm_gpio1_pin_index;
	handle->pwm_gpio2_controller = options->pwm_gpio2_controller;
	handle->pwm_gpio2_pin_index = options->pwm_gpio2_pin_index;

	if(handle->mode == TIMER_COUNTER_MODE_PWM)
	{
		//setup clock selection and mode for wave
		tc_init(handle->timer_module, handle->channel, 2 | TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_ACPA_CLEAR | TC_CMR_ACPC_SET | TC_CMR_BCPB_CLEAR | TC_CMR_BCPC_SET | TC_CMR_ABETRG);
		float multiplier = ((sysclk / 32.0) / 100000.0);
		uint32_t counts = handle->period * multiplier;
		tc_write_rc(handle->timer_module, handle->channel, counts);
		counts = handle->duty_cycle1 * multiplier;
		tc_write_ra(handle->timer_module, handle->channel, counts);
		counts = handle->duty_cycle2 * multiplier;
		tc_write_rb(handle->timer_module, handle->channel, counts);

		if(handle->pwm_gpio1_controller)
		{
			pio_set_peripheral(handle->pwm_gpio1_controller, options->pwm_gpio1_pin_mux_setting, (1<<handle->pwm_gpio1_pin_index));
		}
		if(handle->pwm_gpio2_controller)
		{
			pio_set_peripheral(handle->pwm_gpio2_controller, options->pwm_gpio2_pin_mux_setting, (1<<handle->pwm_gpio2_pin_index));
		}
	}
	else if(handle->mode == TIMER_COUNTER_MODE_TIMER)
	{
		float multiplier = ((sysclk / 32.0) / 100000.0);
		uint32_t counts = handle->period * multiplier;
		tc_init(handle->timer_module, handle->channel, 2 | TC_CMR_CPCTRG);
		tc_write_rc(handle->timer_module, handle->channel, counts);
	}

	//enable interrupts
	if(handle->callback_function)
	{
		tc_enable_interrupt(handle->timer_module, handle->channel, TC_IER_CPCS);
		if(handle->channel == 0)
		{
			NVIC_EnableIRQ(TC0_IRQn);
		}
		else if(handle->channel == 1)
		{
			NVIC_EnableIRQ(TC1_IRQn);	
		}
		else
		{
			NVIC_EnableIRQ(TC2_IRQn);
		}
	}
	else
	{
		tc_disable_interrupt(handle->timer_module, handle->channel, TC_IER_CPCS);
	}

	return handle;
}

bool Timer_Start(timer_counter_t timer)
{
	_timer_counter_t* handle = (_timer_counter_t*) timer;
	tc_start(handle->timer_module, handle->channel);
	return true;
}

bool Timer_Stop(timer_counter_t timer)
{
	_timer_counter_t* handle = (_timer_counter_t*) timer;
	tc_stop(handle->timer_module, handle->channel);

	return true;
}

bool Timer_SetPeriod(timer_counter_t timer, uint32_t period_us)
{
	return true;
}

bool Timer_SetDutyCycle(timer_counter_t timer, uint8_t channel, uint32_t duty_cycle_us)
{
	_timer_counter_t* handle = (_timer_counter_t*) timer;
	uint32_t sysclk = sysclk_get_cpu_hz();
	float multiplier = ((sysclk / 32.0) / 100000.0);
	uint32_t counts = duty_cycle_us * multiplier;

	if(channel == 0)
	{
		tc_write_ra(handle->timer_module, handle->channel, counts);
		handle->duty_cycle1 = duty_cycle_us;
	}
	else
	{
		tc_write_rb(handle->timer_module, handle->channel, counts);
	}
	return true;
}
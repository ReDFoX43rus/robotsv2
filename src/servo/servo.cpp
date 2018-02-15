#include "driver/ledc.h"
#include "esp_err.h"
#include "servo/servo.h"

CServo::CServo(gpio_num_t IN1, gpio_num_t IN2, gpio_num_t IN3, gpio_num_t IN4, gpio_num_t ENA, gpio_num_t ENB){
	m_IN1 = IN1;
	m_IN2 = IN2;
	m_IN3 = IN3;
	m_IN4 = IN4;
	m_ENA = ENA;
	m_ENB = ENB;

	gpio_pad_select_gpio(ENB);
	gpio_pad_select_gpio(ENA);

	gpio_set_direction(ENB, GPIO_MODE_OUTPUT);
	gpio_set_direction(ENA, GPIO_MODE_OUTPUT);

	gpio_set_level(ENA, 1);
	gpio_set_level(ENB, 1);

	m_Inited = true;
}

void CServo::Start(side_t side, direction_t direction, int power){
	if(!m_Inited)
		return;

	int channel;
	gpio_num_t engine;
	if (side == LEFT_HAND){
		channel = 0;
		if (direction == FORWARD){
			engine = m_IN1;
		}
		else {
			engine = m_IN2;
		}
	}
	else{
		channel = 1;
		if (direction == FORWARD){
			engine = m_IN3;
		}
		else {
			engine = m_IN4;
		}
	}



	ledc_timer_config_t ledc_timer;

	ledc_timer.bit_num = LEDC_TIMER_13_BIT;
	ledc_timer.freq_hz = 5000;
	ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;
	ledc_timer.timer_num =  LEDC_TIMER_0;

	ledc_timer_config(&ledc_timer);

	ledc_channel_config_t ledc_channel[2];

	ledc_channel[0].channel = LEDC_CHANNEL_0;
	ledc_channel[0].duty = to_duty(power);
	ledc_channel[0].gpio_num = engine;
	ledc_channel[0].speed_mode = LEDC_HIGH_SPEED_MODE;
	ledc_channel[0].timer_sel =  LEDC_TIMER_0;

	ledc_channel[1].channel = LEDC_CHANNEL_1;
	ledc_channel[1].duty = to_duty(power);
	ledc_channel[1].gpio_num = engine;
	ledc_channel[1].speed_mode = LEDC_HIGH_SPEED_MODE;
	ledc_channel[1].timer_sel = LEDC_TIMER_0;


	ledc_channel_config(&ledc_channel[channel]);

	ledc_fade_func_install(0);

	ledc_set_duty(ledc_channel[channel].speed_mode, ledc_channel[channel].channel, to_duty(power));
	ledc_update_duty(ledc_channel[channel].speed_mode, ledc_channel[channel].channel);
}

void CServo::Stop(){
	if(!m_Inited)
		return;

	gpio_set_level(m_ENA, 0);
	gpio_set_level(m_ENB, 0);

	ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
	ledc_stop(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0);
}

int CServo::to_duty(int percent){
	if (percent > 100){
		return 8192;
	}
	if (percent < 0){
		return 0;
	}

	int duty = 8192*percent/100;

	return duty;

}

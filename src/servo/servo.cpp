#include "driver/ledc.h"
#include "esp_err.h"
#include "servo/servo.h"

CServo::CServo(gpio_num_t PWMA, gpio_num_t DIRA, gpio_num_t PWMB, gpio_num_t DIRB){
	m_PWMA = PWMA;
	m_DIRA = DIRA;
	m_PWMB = PWMB;
	m_DIRB = DIRB;

	gpio_pad_select_gpio(DIRA);
	gpio_pad_select_gpio(DIRB);
	gpio_set_direction(DIRA, GPIO_MODE_OUTPUT);
	gpio_set_direction(DIRB, GPIO_MODE_OUTPUT);
	gpio_set_level(DIRA, 0);
	gpio_set_level(DIRB, 0);

	m_Inited = false;
}

void CServo::Init(){
	ledc_timer_config_t ledc_timer;
	ledc_timer.bit_num = LEDC_TIMER_13_BIT;
	ledc_timer.freq_hz = 5000;
	ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;
	ledc_timer.timer_num =  LEDC_TIMER_0;
	ledc_timer_config(&ledc_timer);

	ledc_channel_config_t ledc_channel;
	ledc_channel.duty = 0;
	ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
	ledc_channel.timer_sel =  LEDC_TIMER_0;

	ledc_channel.channel = LEDC_CHANNEL_0;
	ledc_channel.gpio_num = m_PWMA;
	ledc_channel_config(&ledc_channel);

	ledc_channel.channel = LEDC_CHANNEL_1;
	ledc_channel.gpio_num = m_PWMB;
	ledc_channel_config(&ledc_channel);

	m_Inited = true;
}

void CServo::Start(side_t side, direction_t direction, int power){
	if(!m_Inited)
		Init();

	ledc_channel_t channel;
	if (side == LEFT_HAND){
		channel = LEDC_CHANNEL_0;
		if (direction == FORWARD){
			gpio_set_level(m_DIRA, 0);
		}
		else {
			gpio_set_level(m_DIRA, 1);
		}
	}
	else{
		channel = LEDC_CHANNEL_0;//LEDC_CHANNEL_1;
		if (direction == FORWARD){
			gpio_set_level(m_DIRB, 0);
		}
		else {
			gpio_set_level(m_DIRB, 1);
		}
	}

	ledc_set_duty(LEDC_HIGH_SPEED_MODE, channel, to_duty(power));
	ledc_update_duty(LEDC_HIGH_SPEED_MODE, channel);
}

void CServo::Stop(){
	if(!m_Inited)
		Init();

	gpio_set_level(m_DIRA, 0);
	gpio_set_level(m_DIRB, 0);

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

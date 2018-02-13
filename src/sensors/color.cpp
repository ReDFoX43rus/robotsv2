#include "color.h"

CColorSensor::CColorSensor(gpio_num_t s0, gpio_num_t s1, gpio_num_t s2,
			gpio_num_t s3, adc1_channel_t out):m_S0(s0), m_S1(s1), m_S2(s2),
			m_S3(s3), m_Out(out) {
	gpio_pad_select_gpio(s0);
	gpio_pad_select_gpio(s1);
	gpio_pad_select_gpio(s2);
	gpio_pad_select_gpio(s3);

	gpio_set_direction(s0, GPIO_MODE_OUTPUT);
	gpio_set_direction(s1, GPIO_MODE_OUTPUT);
	gpio_set_direction(s2, GPIO_MODE_OUTPUT);
	gpio_set_direction(s3, GPIO_MODE_OUTPUT);

	adc.SetupAdc((int)out, 1100);
}

uint8_t CColorSensor::GetColor(Color color){
	switch(color){
		case RED:
			gpio_set_level(m_S2, 0);
			gpio_set_level(m_S3, 0);
			break;
		case GREEN:
			gpio_set_level(m_S2, 1);
			gpio_set_level(m_S3, 1);
			break;
		case BLUE:
			gpio_set_level(m_S2, 0);
			gpio_set_level(m_S3, 1);
			break;
		default:
			gpio_set_level(m_S2, 1);
			gpio_set_level(m_S3, 0);
			break;
	}

	uint32_t v = adc.GetVoltage((int)m_Out);

	return (v*255)/1100;
}

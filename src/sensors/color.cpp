#include "color.h"

CColorSensor::CColorSensor(){
	m_Adc = new CAdc();
}

CColorSensor::~CColorSensor(){
	delete m_Adc;
}

void CColorSensor::Setup(gpio_num_t s0, gpio_num_t s1, gpio_num_t s2,
			gpio_num_t s3, gpio_num_t out){

	if(m_S0 == s0 && m_S1 == s1 && m_S2 == s2 && m_S3 == s3 && m_Out == out)
		return;

	m_S0 = s0;
	m_S1 = s1;
	m_S2 = s2;
	m_S3 = s3;
	m_Out = out;

	gpio_pad_select_gpio(s0);
	gpio_pad_select_gpio(s1);
	gpio_pad_select_gpio(s2);
	gpio_pad_select_gpio(s3);

	gpio_set_direction(s0, GPIO_MODE_OUTPUT);
	gpio_set_direction(s1, GPIO_MODE_OUTPUT);
	gpio_set_direction(s2, GPIO_MODE_OUTPUT);
	gpio_set_direction(s3, GPIO_MODE_OUTPUT);

	gpio_set_level(s0, 1);
	gpio_set_level(s1, 1);
	// m_Adc->SetupAdc((int)out, 1100);
}

extern void freq_counter_pcnt_init(uint32_t gpio_num);
extern uint32_t freq_measure();

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

	//uint32_t v = m_Adc->GetVoltage((int)m_Out);
	freq_counter_pcnt_init(m_Out);
	return freq_measure();

	//return (v*255)/1100;
}

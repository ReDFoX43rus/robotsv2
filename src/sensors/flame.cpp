#include "flame.h"

void CFlame::SetupDigOuput(gpio_num_t gpio){
	gpio_pad_select_gpio(gpio);
	gpio_set_direction(gpio, GPIO_MODE_INPUT);
	m_DigOutput = gpio;
}

void CFlame::SetupAdcChannel(adc1_channel_t channel){
	m_AdcChannel = channel;
}

int CFlame::GetDigValue(){
	return gpio_get_level(m_DigOutput);
}

int CFlame::GetAnValue(){
	CAdc adc = CAdc();
	adc.SetupAdc(m_AdcChannel, 1100);
	return adc.GetVoltage(m_AdcChannel);
}

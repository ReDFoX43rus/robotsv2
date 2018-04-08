#include "noise.h"

CNoise& CNoise::Setup(gpio_num_t pin){
	gpio_pad_select_gpio(pin);
	gpio_set_direction(pin, GPIO_MODE_INPUT);

	m_Pin = pin;
	return *this;
}

uint8_t CNoise::GetData() {
	return gpio_get_level(m_Pin);
}

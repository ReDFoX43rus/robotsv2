#ifndef SENSORS_FLAME_H
#define SENSORS_FLAME_H

#include "driver/gpio.h"
#include "adc.h"

class CFlame {
public:
	void SetupDigOuput(gpio_num_t dig);
	void SetupAdcChannel(adc1_channel_t channel);
	int GetDigValue();
	int GetAnValue();
private:
	gpio_num_t m_DigOutput;
	adc1_channel_t m_AdcChannel;
};


#endif /* end of include guard: SENSORS_FLAME_H */

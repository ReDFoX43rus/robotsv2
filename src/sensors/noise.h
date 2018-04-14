#ifndef SENSORS_NOISE_H
#define SENSORS_NOISE_H

#include "driver/gpio.h"

class CNoise {
public:
	CNoise& Setup(gpio_num_t pin);
	uint8_t GetData();
private:
	gpio_num_t m_Pin;
};

#endif /* end of include guard: SENSORS_NOISE_H */

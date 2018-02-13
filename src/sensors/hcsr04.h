#ifndef SENSORS_HCSR04_H
#define SENSORS_HCSR04_H

#include "driver/gpio.h"

class HCSR04{
public:
	HCSR04(gpio_num_t echo, gpio_num_t trig);
	~HCSR04(){}

	int GetDistance();

private:
	gpio_num_t m_Echo;
	gpio_num_t m_Trig;
};

#endif /* end of include guard: SENSORS_HCSR04_H */

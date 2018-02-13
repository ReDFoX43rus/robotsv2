#ifndef SENSORS_LINE_DIG_H
#define SENSORS_LINE_DIG_H

#include "driver/gpio.h"

class CLineDig{
public:
	CLineDig(gpio_num_t output);
	int GetData();

private:
	gpio_num_t m_Output;
};

#endif /* end of include guard: SENSORS_LINE_DIG_H */

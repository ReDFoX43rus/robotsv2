#ifndef SENSORS_LINE_DIG_H
#define SENSORS_LINE_DIG_H

#include "driver/gpio.h"

class CLineDig{
public:
	int GetData(gpio_num_t output));
};

#endif /* end of include guard: SENSORS_LINE_DIG_H */

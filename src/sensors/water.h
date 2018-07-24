#ifndef SENSORS_WATER_H
#define SENSORS_WATER_H

#include "driver/gpio.h"

class CWater {
public:
	int GetData(gpio_num_t output);
};

#endif /* end of include guard: SENSORS_LINE_WATER_H */

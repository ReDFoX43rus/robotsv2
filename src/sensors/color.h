#ifndef SENSORS_COLOR_H
#define SENSORS_COLOR_H

#include "driver/gpio.h"
#include "adc.h"
#include "drivers/pulse_handler.h"

class CColorSensor{
public:
	CColorSensor();
	~CColorSensor();

	void Setup(gpio_num_t s0, gpio_num_t s1, gpio_num_t s2, gpio_num_t s3, gpio_num_t out);

	enum Color{
		RED,
		GREEN,
		BLUE,
		CLEAR
	};

	uint8_t GetColor(Color color);
private:
	gpio_num_t m_S0;
	gpio_num_t m_S1;
	gpio_num_t m_S2;
	gpio_num_t m_S3;
	gpio_num_t m_Out;

	CPulseHandler *m_PulseHandler;
	CAdc *m_Adc;
};

#endif /* end of include guard: SENSORS_COLOR_H */

#ifndef SENSORS_SHARP_2Y0A21_H
#define SENSORS_SHARP_2Y0A21_H

#include "adc.h"
#include "math.h"


class SHARP_2Y0A21  {
public:
	SHARP_2Y0A21(int pin);
	~SHARP_2Y0A21();
	int GetDistance();

private:
	int m_pin;
	CAdc *adc;

};

#endif

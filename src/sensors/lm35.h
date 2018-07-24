#ifndef SENSORS_LM35_H
#define SENSORS_LM35_H

#include "adc.h"

class LM35 {
public:
	LM35();
	~LM35();
	int GetTemperature();
	void ChangePin(int pin);

private:
	int m_pin;
	CAdc *adc;

};

#endif

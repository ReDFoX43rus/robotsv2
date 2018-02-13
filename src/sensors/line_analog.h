#ifndef SENSORS_LINE_ANALOG_H
#define SENSORS_LINE_ANALOG_H

#include "adc.h"

class CLineAn{
public:
	CLineAn(int pin): m_Pin(pin){}

	int GetData();
private:
	int m_Pin;
};

#endif /* end of include guard: SENSORS_LINE_ANALOG_H */

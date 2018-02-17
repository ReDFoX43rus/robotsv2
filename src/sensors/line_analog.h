#ifndef SENSORS_LINE_ANALOG_H
#define SENSORS_LINE_ANALOG_H

#include "adc.h"

class CLineAn{
public:
	CLineAn();
	~CLineAn();

	int GetData(int pin);
private:
	CAdc *m_Adc;
	int m_Pin;
};

#endif /* end of include guard: SENSORS_LINE_ANALOG_H */

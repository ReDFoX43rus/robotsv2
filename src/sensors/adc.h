#ifndef SENSORS_ADC_H
#define SENSORS_ADC_H

#include "driver/adc.h"
#include "esp_adc_cal.h"

class CAdc{
public:
	CAdc();
	~CAdc(){}

	// See driver/include/driver/adc.h to understand adcNum <-> GPIO binding
	// maxVoltage in mV
	bool SetupAdc(int adcNum, uint32_t maxVoltage);
	uint32_t GetVoltage(int adcNum);
private:
	esp_adc_cal_characteristics_t m_Characteristics[ADC1_CHANNEL_MAX];
};

#endif /* end of include guard: SENSORS_ADC_H */

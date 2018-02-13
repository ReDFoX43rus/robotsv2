#include "adc.h"

CAdc::CAdc(){
	adc1_config_width(ADC_WIDTH_12Bit);
}

bool CAdc::SetupAdc(int adcNum, uint32_t maxVoltage){
	if(adcNum < 0 || adcNum >= ADC1_CHANNEL_MAX)
		return false;

	adc_atten_t atten = ADC_ATTEN_0db;
	if(maxVoltage <= 1100)
		atten = ADC_ATTEN_0db;
	else if(maxVoltage <= 1500)
		atten = ADC_ATTEN_2_5db;
	else if(maxVoltage <= 2200)
		atten = ADC_ATTEN_6db;
	else atten = ADC_ATTEN_11db;

	adc1_config_channel_atten((adc1_channel_t)adcNum, atten);

	esp_adc_cal_get_characteristics(maxVoltage, atten, ADC_WIDTH_12Bit, &m_Characteristics[adcNum]);
	return true;
}

uint32_t CAdc::GetVoltage(int adcNum){
	if(adcNum < 0 || adcNum >= ADC1_CHANNEL_MAX)
		return 0;

	return adc1_to_voltage((adc1_channel_t)adcNum, &m_Characteristics[adcNum]);
}

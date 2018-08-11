#include "iobase/iobase.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define DEFAULT_VREF 1092
#define ACS_ZERO (3350/2)

void CmdAcs712Handler(CIOBase &io, int argc, char *argv[]){
	uint32_t vref = atoi(argv[1]);

	esp_adc_cal_characteristics_t adc_chars;// = malloc(sizeof(esp_adc_cal_characteristics_t));
	esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, vref, &adc_chars);
	//Check type of calibration value used to characterize ADC
	if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF)
		io << "eFuse vFef" << endl;
	else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP)
		io << "Two point" << endl;
	else
		io << "Default" << endl;

	for(int i = 0; i < 10; i++){
		uint32_t reading =  adc1_get_raw(ADC1_CHANNEL_4);
		uint32_t voltage = esp_adc_cal_raw_to_voltage(reading, &adc_chars);

		int current = (ACS_ZERO - voltage) / 100;

		io << "Reading: " << reading << " Voltage: " << voltage << " Current: " << current << endl;

		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
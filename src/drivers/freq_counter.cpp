#include "driver/gpio.h"
#include "driver/pcnt.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "time.h"

#include "uart.h"

void freq_counter_pcnt_init(uint32_t gpio_num){
	pcnt_config_t cfg = {
		.pulse_gpio_num = 4,
		.ctrl_gpio_num = PCNT_PIN_NOT_USED,
		.lctrl_mode = PCNT_MODE_KEEP,
		.hctrl_mode = PCNT_MODE_KEEP,
		.pos_mode = PCNT_COUNT_INC,
		.neg_mode = PCNT_COUNT_DIS,
		.counter_h_lim = 30000,
		.counter_l_lim = 0,
		.unit = PCNT_UNIT_0,
		.channel = PCNT_CHANNEL_0
	};

	if(pcnt_unit_config(&cfg) != ESP_OK){
		uart << "Error initing pcnt" << endl;
		return;
	}

	// pcnt_set_filter_value(PCNT_UNIT_0, 100);

	pcnt_counter_pause(PCNT_UNIT_0);
	pcnt_counter_clear(PCNT_UNIT_0);
}

uint32_t freq_measure(){
	short value;

	uint32_t stamp = clock();
	pcnt_counter_resume(PCNT_UNIT_0);

	vTaskDelay(pdMS_TO_TICKS(100));

	pcnt_get_counter_value(PCNT_UNIT_0, &value);
	uint32_t stamp2 = clock();
	pcnt_counter_pause(PCNT_UNIT_0);
	pcnt_counter_clear(PCNT_UNIT_0);

	uart << "Stamp1: " << stamp << " Stamp2: " << stamp2 << endl;

	uint32_t result = (stamp2 - stamp)*1000/CLOCKS_PER_SEC;
	if(!result)
		return 0;

	uart << "Edges: " << value << " Time: " << result << " freq: " << (value/result) << endl;
	return value/result;
}

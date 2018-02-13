#include "../../iobase/uart/uart.h"
#include "../../sensors/flame.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* TODO: Check analog value */
void CmdFlameHandler(int argc, char **argv){
	if(argc < 3){
		uart << "Usage: flame dig_pin an_pin" << endl;
		return;
	}

	CFlame flame;
	flame.SetupDigOuput((gpio_num_t)atoi(argv[1]));
	flame.SetupAdcChannel((adc1_channel_t)atoi(argv[2]));

	while(1){
		int dig = flame.GetDigValue();
		int an = flame.GetAnValue();

		uart << "Digital value: " << dig << " Analog value: " << an << endl;
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

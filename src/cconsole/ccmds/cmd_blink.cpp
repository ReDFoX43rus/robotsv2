#include <iobase/iobase.h>
#include <driver/gpio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void CmdBlinkHandler(CIOBase &io, int argc, char *argv[]){
	if(argc != 2){
		io << "Usage: " << argv[1] << " io_num" << endl;
		return;
	}

	int num = atoi(argv[1]);
	gpio_num_t gpio = (gpio_num_t)num;
	gpio_set_direction(gpio, GPIO_MODE_OUTPUT);

	bool level = false;

	while(1){
		level = !level;
		gpio_set_level(gpio, level);

		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
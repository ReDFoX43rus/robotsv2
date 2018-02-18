#include "../../iobase/uart/uart.h"
#include "../../sensors/hcsr04.h"

#include "driver/gpio.h"

void CmdHcsr04Handler(int argc, char **argv){
	HCSR04 sensor = HCSR04();
	sensor.Setup(GPIO_NUM_4, GPIO_NUM_5);
	int distance = sensor.GetDistance();

	uart << "Distance: " << distance << endl;
}

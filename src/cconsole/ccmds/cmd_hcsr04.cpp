#include "iobase/iobase.h"
#include "sensors/hcsr04.h"

#include "driver/gpio.h"

void CmdHcsr04Handler(CIOBase &io, int argc, char **argv){
	HCSR04 sensor = HCSR04();
	sensor.Setup(GPIO_NUM_4, GPIO_NUM_5);
	int distance = sensor.GetDistance();

	io << "Distance: " << distance << endl;
}

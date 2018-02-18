#include "../../iobase/uart/uart.h"
#include "../../sensors/sharp_2y0a21.h"

#include "driver/adc.h"
#include "unistd.h"

void CmdSharpHandler(int argc, char **argv){
	SHARP_2Y0A21 sensor = SHARP_2Y0A21();
	sensor.ChangePin(ADC1_CHANNEL_4);
	while (true) {
		int distance = sensor.GetDistance();
		uart << "Distance: " << distance << endl;
		usleep(200*1000);
	}
}

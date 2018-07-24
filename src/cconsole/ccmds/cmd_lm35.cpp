#include "iobase/iobase.h"
#include "sensors/lm35.h"

#include "driver/adc.h"
#include "unistd.h"

void CmdLm35Handler(CIOBase &io, int argc, char **argv){
	LM35 sensor = LM35();
	sensor.ChangePin(ADC1_CHANNEL_4);
	while (true) {
		int temperature = sensor.GetTemperature();
		io << "Temperature: " << temperature << endl;
		usleep(200*1000);
	}
}

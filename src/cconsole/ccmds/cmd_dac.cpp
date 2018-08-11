#include "iobase/iobase.h"
#include "driver/gpio.h"
#include "driver/dac.h"

void CmdDacHandler(CIOBase &io, int argc, char *argv[]){
	int value = atoi(argv[1]);

	dac_output_enable(DAC_CHANNEL_1);
	dac_output_voltage(DAC_CHANNEL_1, value);
}
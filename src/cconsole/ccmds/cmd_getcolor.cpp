#include "uart.h"
#include "sensors/color.h"

static CColorSensor cs = CColorSensor();
void CmdGetcolorHandler(int argc, char **argv){
	cs.Setup(GPIO_NUM_14, GPIO_NUM_15, GPIO_NUM_16, GPIO_NUM_17, ADC1_CHANNEL_4);
	for(int i = 0; i < 10000; i++){
		uint8_t color = cs.GetColor((CColorSensor::Color)atoi(argv[1]));
		uart << "Color: " << color << endl;
	}
}

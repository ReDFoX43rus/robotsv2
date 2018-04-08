#include "iobase/iobase.h"
#include "sensors/color.h"

#include "time.h"

static CColorSensor cs = CColorSensor();
void CmdGetcolorHandler(CIOBase &io, int argc, char **argv){
	cs.Setup(GPIO_NUM_14, GPIO_NUM_15, GPIO_NUM_16, GPIO_NUM_17, GPIO_NUM_18);
	for(int i = 0; i < 10000; i++){
		uint8_t red = cs.GetColor(CColorSensor::RED);
		uint8_t green = cs.GetColor(CColorSensor::GREEN);
		uint8_t blue = cs.GetColor(CColorSensor::BLUE);
		uint8_t clear = cs.GetColor(CColorSensor::CLEAR);

		io << "R: " << red << " G: " << green << " B: " << blue << " Clear: " << clear << endl;
	}
}

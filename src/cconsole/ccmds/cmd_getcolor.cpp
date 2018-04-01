#include "iobase/iobase.h"
#include "sensors/color.h"

#include "time.h"

static CColorSensor cs = CColorSensor();
void CmdGetcolorHandler(CIOBase &io, int argc, char **argv){
	cs.Setup(GPIO_NUM_14, GPIO_NUM_15, GPIO_NUM_16, GPIO_NUM_17, GPIO_NUM_18);
	// for(int i = 0; i < 10000; i++){
	// 	uint8_t color = cs.GetColor((CColorSensor::Color)atoi(argv[1]));
	// 	io << "Color: " << color << endl;
	// }

	gpio_pad_select_gpio(GPIO_NUM_4);

	gpio_set_direction(GPIO_NUM_4, GPIO_MODE_INPUT);

	int lastlevel = 0, level = 0;
	uint32_t last_time = clock(), curtime = clock();
	while(1){
		level = gpio_get_level(GPIO_NUM_4);
		curtime = clock();
		if(level == lastlevel)
			continue;

		lastlevel = level;
		io << "Level: " << level << " Time: " << (curtime - last_time)*1000/CLOCKS_PER_SEC << endl;
		last_time = clock();
	}
}

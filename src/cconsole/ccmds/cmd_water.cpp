#include "iobase/iobase.h"
#include "unistd.h"
#include "sensors/water.h"

static CWater water = CWater();

void CmdWaterHandler(CIOBase &io, int argc, char **argv){
	for(int i = 0; i < 250; i++) {
		io << water.GetData(GPIO_NUM_18) << endl;
		usleep(50*1000);
	}
}

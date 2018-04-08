#include "iobase/iobase.h"
#include "sensors/noise.h"

CNoise noise;
void CmdNoisetestHandler(CIOBase &io, int argc, char *argv[]){
	noise.Setup(GPIO_NUM_13);

	uint8_t level;
	for(int i = 0; i < 10000; i++){
		level = noise.GetData();
		io << "Noise more than threshhold: " << level << endl;
	}
}

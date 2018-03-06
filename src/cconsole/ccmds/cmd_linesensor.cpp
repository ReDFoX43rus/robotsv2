#include "iobase/iobase.h"
#include "unistd.h"
#include "sensors/line_analog.h"
#include "sensors/line_dig.h"

static CLineAn line_analog = CLineAn();
static CLineDig line_dig = CLineDig();

void CmdLinesensorHandler(CIOBase &io, int argc, char **argv){
	if(argc == 1){
		for(int i = 0; i < 250; i++){
			io << line_analog.GetData(4) << endl;
			usleep(50*1000);
		}
	} else {
		for(int i = 0; i < 250; i++){
			io << line_dig.GetData(GPIO_NUM_18) << endl;
			usleep(50*1000);
		}
	}
}

#include "uart.h"
#include "unistd.h"
#include "sensors/line_analog.h"
#include "sensors/line_dig.h"

static CLineAn line_analog = CLineAn(4);
static CLineDig line_dig = CLineDig(GPIO_NUM_18);

void CmdLinesensorHandler(int argc, char **argv){
	if(argc == 1){
		for(int i = 0; i < 250; i++){
			uart << line_analog.GetData() << endl;
			usleep(50*1000);
		}
	} else {
		for(int i = 0; i < 250; i++){
			uart << line_dig.GetData() << endl;
			usleep(50*1000);
		}
	}
}

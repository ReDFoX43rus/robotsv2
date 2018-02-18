#include "uart.h"
#include "unistd.h"
#include "sensors/line_analog.h"
#include "sensors/line_dig.h"

static CLineAn line_analog = CLineAn();
static CLineDig line_dig = CLineDig();

void CmdLinesensorHandler(int argc, char **argv){
	if(argc == 1){
		for(int i = 0; i < 250; i++){
			uart << line_analog.GetData(4) << endl;
			usleep(50*1000);
		}
	} else {
		for(int i = 0; i < 250; i++){
			uart << line_dig.GetData(GPIO_NUM_18) << endl;
			usleep(50*1000);
		}
	}
}

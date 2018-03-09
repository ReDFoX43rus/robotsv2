#include "iobase/iobase.h"
#include "wifi/wifi.h"
#include "string.h"

static CWifi wifi;

void CmdWifiHandler(CIOBase &io, int argc, char **argv){
	if(argc < 2){
		io << "Usage: wifi [cmd]" << endl;
	}

	if(!strcmp(argv[1], "init")) {
		io << "wifi init: " << wifi.Init() << endl;
	}
	else if(!strcmp(argv[1], "scan")){
		int res;
		for(int i = 1; i <= 13; i++){
			res = wifi.Search(i);
			if(!res)
				wifi.ListScanRecords();
		}
	}
	else if(!strcmp(argv[1], "connect")){
		io << "wifi connect: " << wifi.Connect(argv[2], argv[3]);
	}
}

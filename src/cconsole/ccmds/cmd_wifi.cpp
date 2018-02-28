#include "uart.h"
#include "wifi/wifi.h"
#include "string.h"

static CWifi wifi;

void CmdWifiHandler(int argc, char **argv){
	// int res = wifi.Init();
	// uart << "wifi inited: " << res << endl;
    //
	// for(int i = 1; i <= 13; i++){
	// 	res = wifi.Search(i);
	// 	if(!res)
	// 		wifi.ListScanRecords();
	// }

	if(argc < 2){
		uart << "Usage: wifi [cmd]" << endl;
	}

	if(!strcmp(argv[1], "init"))
		uart << "wifi init: " << wifi.Init() << endl;
	else if(!strcmp(argv[1], "scan")){
		int res;
		for(int i = 1; i <= 13; i++){
			res = wifi.Search(i);
			if(!res)
				wifi.ListScanRecords();
		}
	}
	else if(!strcmp(argv[1], "connect")){
		uart << "wifi connect: " << wifi.Connect(argv[2], argv[3]);
	}
}

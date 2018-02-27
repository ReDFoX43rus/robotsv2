#include "uart.h"

#include "wifi/wifi.h"
void CmdWifiHandler(int argc, char **argv){
	int result = wifi_init_sta("DIR-300A", "12169931");
	uart << "wifi_init: " << result << endl;
}

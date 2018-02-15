#include "handler_c.h"
#include "uart.h"

extern "C" int handle_sensor(int sensor_id, const int *data){
	uart << "sensor_id: " << sensor_id << endl;

	uart << "data: ";
	for(int i = 0; i < 3; i++)
		uart << data[i] << " ";
	uart << endl;

	return 0;
}

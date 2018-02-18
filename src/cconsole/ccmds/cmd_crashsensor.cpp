#include "../../sensors/crash.h"
#include "../../iobase/uart/uart.h"

#include "driver/gpio.h"

static CrashSensor sensor = CrashSensor().ChangeTrigger(GPIO_NUM_4);

void CrashCallback(void*){
	uart << "Crash occurred :c" << endl;
	sensor.Disable();
}

void CmdCrashsensorHandler(int argc, char **argv){
	sensor.Setup().SetCallbackFunction(&CrashCallback);
	uart << "Setup ok ;)" << endl;
}

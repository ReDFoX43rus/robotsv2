#include "sensors/crash.h"
#include "iobase/uart/uart.h"

#include "driver/gpio.h"

static CrashSensor sensor = CrashSensor().ChangeTrigger(GPIO_NUM_4).Setup();

void CrashCallback(void*){
	uart << "Crash occurred :c" << endl;
	sensor.Disable();
}

void CmdCrashsensorHandler(CIOBase &io, int argc, char **argv){
	sensor.Setup().SetCallbackFunction(&CrashCallback);
	io << "Setup ok ;)" << endl;
}

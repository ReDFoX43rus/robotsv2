#include "iobase/iobase.h"
#include "sensors/trh/ds18b20.h"

static CDs18b20 sensor(GPIO_NUM_4);

void CmdDs18B20Handler(CIOBase &io, int argc, char *argv[]){
    float temp = 0;
    bool result = sensor.GetTemperature(&temp);

    io << "My temp: " << (int) temp << " result: " << result << endl;
}
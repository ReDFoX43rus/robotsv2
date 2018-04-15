#include "iobase/iobase.h"

#include "sensors/trh/dht11.h"

CDht11 dht11 = CDht11(GPIO_NUM_4);

void CmdDht11Handler(CIOBase &io, int argc, char *argv[]){
    dht11_response_t res = dht11.Measure();

    io << "Temp: " << (int)res.temp1 << "." << (int)res.temp2 << 
        " RH: " << res.rh1 << "." << res.rh2 << 
        " OK: " << res.ok << endl;
}
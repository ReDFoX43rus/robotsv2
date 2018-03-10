#include "iobase/iobase.h"
#include "sensors/trh/am2320.h"

void CmdAm2320Handler(CIOBase &io, int argc, char *argv[]){
	CAm2320 am;
	am.Measure();

	am2320_ret_t ret = am.GetData();

	io << "Temperature: " << ret.temperature << " RH: " << ret.humidity << " Ok: " << ret.ok << endl;
}

#include <iobase/iobase.h>
#include <sensors/compass.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static CHMC5883L cmps;
void CmdCompassHandler(CIOBase &io, int argc, char **argv){
	cmps.Configure(G_DEFAULT, S1090);

	for(int i = 0; i < 100; i++){
		compass_data_t data = cmps.GetPosition();
		io << "x: " << data.x << " y: " << data.y << " z: " << data.z << " angle: " << data.angle << endl;
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

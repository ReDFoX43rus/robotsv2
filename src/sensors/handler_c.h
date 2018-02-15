#ifndef SENSORS_HANDLER_C_H
#define SENSORS_HANDLER_C_H

extern "C" {

enum SENSOR_IDS{
	SENSOR_LINE_AN,
	MAX_SENSORS
};

int handle_sensor(int sensor_id, const int *data);

}

#endif /* end of include guard: SENSORS_HANDLER_C_H */

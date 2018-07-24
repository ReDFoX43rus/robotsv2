/* Unified place to get sensor data from C code
 * This is needed for RuC and lua to get data */

#ifndef SENSORS_HANDLER_C_H
#define SENSORS_HANDLER_C_H

#ifdef __cplusplus
extern "C" {
#endif

enum SENSOR_IDS{
	SENSOR_COLOR_RED=0,
	SENSOR_COLOR_GREEN,
	SENSOR_COLOR_BLUE,
	SENSOR_COMPASS_X,
	SENSOR_COMPASS_Y,
	SENSOR_COMPASS_Z,
	SENSOR_COMPASS_ANGLE,
	SENSOR_LINE_AN,
	SENSOR_FLAME_AN,
	SENSOR_SHARP, // distance sensor IR
	SENSOR_SOUND,
	SENSOR_CRASH_BUTTON,
	SENSOR_HCSR04,
	SENSOR_LM35,
	SENSOR_WATER,
	MAX_SENSORS,
	SENSOR_LINE_DIG = -SENSOR_LINE_AN,
	SENSOR_FLAME_DIG = -SENSOR_FLAME_AN
};

int handle_sensor(int sensor_id, const int *data);
void set_voltage(int pin, int level);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: SENSORS_HANDLER_C_H */

#include "handler_c.h"
#include "uart.h"

#include "driver/gpio.h"

#include "line_analog.h"
#include "color.h"
#include "compass.h"
#include "crash.h"
#include "flame.h"
#include "hcsr04.h"
#include "line_dig.h"

static bool crash_notifier = 0;
static inline void crash_sensor_callback(void*);

extern "C" int handle_sensor(int sensor_id, const int *data){
	uart << "sensor_id: " << sensor_id << endl;

	uart << "data: ";
	for(int i = 0; i < 3; i++)
		uart << data[i] << " ";
	uart << endl;

	if(sensor_id < 0 || sensor_id >= MAX_SENSORS)
		return 0;

	static CColorSensor color_sensor;
	static CHMC5883L compass;
	compass.Configure(G_DEFAULT, S1090);
	static CFlame flame_sensor;

	switch(sensor_id){
		case SENSOR_LINE_AN:
			static CLineAn line_analog = CLineAn();
			return line_analog.GetData(data[0]);
		case SENSOR_COLOR_RED:
			color_sensor.Setup((gpio_num_t)data[0], (gpio_num_t)data[1], (gpio_num_t)data[2], (gpio_num_t)data[3], (gpio_num_t)data[4]);
			return color_sensor.GetColor(CColorSensor::RED);
		case SENSOR_COLOR_GREEN:
			color_sensor.Setup((gpio_num_t)data[0], (gpio_num_t)data[1], (gpio_num_t)data[2], (gpio_num_t)data[3], (gpio_num_t)data[4]);
			return color_sensor.GetColor(CColorSensor::GREEN);
		case SENSOR_COLOR_BLUE:
			color_sensor.Setup((gpio_num_t)data[0], (gpio_num_t)data[1], (gpio_num_t)data[2], (gpio_num_t)data[3], (gpio_num_t)data[4]);
			return color_sensor.GetColor(CColorSensor::RED);
		case SENSOR_COMPASS_X:
		case SENSOR_COMPASS_Y:
		case SENSOR_COMPASS_Z:
		case SENSOR_COMPASS_ANGLE:
			compass_data_t compass_data = compass.GetPosition();
			if(sensor_id == SENSOR_COMPASS_X)
				return compass_data.x;
			else if(sensor_id == SENSOR_COMPASS_Y)
				return compass_data.y;
			else if(sensor_id == SENSOR_COMPASS_Z)
				return compass_data.z;
			else return compass_data.angle;
			break;
		case SENSOR_CRASH_BUTTON:
			static CrashSensor crash_sensor;
			crash_sensor.ChangeTrigger((gpio_num_t)data[0]).Setup().SetCallbackFunction(crash_sensor_callback);
			if(crash_notifier){
				crash_sensor.Disable();
				crash_notifier = false;
				return 1;
			}
			return 0;
		case SENSOR_FLAME_DIG:
			flame_sensor.SetupDigOuput((gpio_num_t) data[0]);
			return flame_sensor.GetDigValue();
		case SENSOR_FLAME_AN:
			flame_sensor.SetupAdcChannel((adc1_channel_t) data[0]);
			return flame_sensor.GetAnValue();
		case SENSOR_HCSR04:
			static HCSR04 distance_sensor;
			distance_sensor.Setup((gpio_num_t)data[0], (gpio_num_t)data[1]);
			return distance_sensor.GetDistance();
		case SENSOR_LINE_DIG:
			static CLineDig line_digital;
			return line_digital.GetData((gpio_num_t)data[0]);
		case SENSOR_SHARP:
			static SHARP_2Y0A21 sharp_sensor;
			sharp_sensor.ChangePin(data[0]);
			return sharp_sensor.GetDistance();
	}

	return 0;
}

inline void crash_sensor_callback(void*){
	crash_notifier = true;
}

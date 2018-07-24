#include "handler_c.h"
#include "uart.h"

#include "driver/gpio.h"
#include "driver/ledc.h"

#include "line_analog.h"
#include "color.h"
#include "compass.h"
#include "crash.h"
#include "flame.h"
#include "hcsr04.h"
#include "line_dig.h"
#include "sharp_2y0a21.h"

#include "uart.h"

static bool crash_notifier = 0;
static inline void crash_sensor_callback(void*);

/* map for pin to channel converting
 * 0 - LEDC_CHANNEL_2
 * 1 - LEDC_CHANNEL_3
 * 2 - LEDC_CHANNEL_4
 * 3 - LEDC_CHANNEL_5
 * 4 - LEDC_CHANNEL_6
 * 5 - LEDC_CHANNEL_7 */
static int ledc_channels[6] = {-1,-1,-1,-1,-1,-1};

extern "C" int handle_sensor(int sensor_id, const int *data){
	if(sensor_id >= MAX_SENSORS)
		return -0x20;

	static CColorSensor color_sensor;
	static CHMC5883L compass;
	compass.Configure(G_DEFAULT, S1090);
	static CFlame flame_sensor;

	switch(sensor_id){
		case SENSOR_LINE_AN: {
			static CLineAn line_analog = CLineAn();
			return line_analog.GetData(data[0]);
		}
		case SENSOR_COLOR_RED: {
			color_sensor.Setup((gpio_num_t)data[0], (gpio_num_t)data[1], (gpio_num_t)data[2], (gpio_num_t)data[3], (gpio_num_t)data[4]);
			return color_sensor.GetColor(CColorSensor::RED);
		}
		case SENSOR_COLOR_GREEN: {
			color_sensor.Setup((gpio_num_t)data[0], (gpio_num_t)data[1], (gpio_num_t)data[2], (gpio_num_t)data[3], (gpio_num_t)data[4]);
			return color_sensor.GetColor(CColorSensor::GREEN);
		}
		case SENSOR_COLOR_BLUE: {
			color_sensor.Setup((gpio_num_t)data[0], (gpio_num_t)data[1], (gpio_num_t)data[2], (gpio_num_t)data[3], (gpio_num_t)data[4]);
			return color_sensor.GetColor(CColorSensor::RED);
		}
		case SENSOR_COMPASS_X:
		case SENSOR_COMPASS_Y:
		case SENSOR_COMPASS_Z:
		case SENSOR_COMPASS_ANGLE: {
			compass_data_t compass_data = compass.GetPosition();
			if(sensor_id == SENSOR_COMPASS_X)
				return compass_data.x;
			else if(sensor_id == SENSOR_COMPASS_Y)
				return compass_data.y;
			else if(sensor_id == SENSOR_COMPASS_Z)
				return compass_data.z;
			else return compass_data.angle;
			break;
		}
		case SENSOR_CRASH_BUTTON: {
			static CrashSensor crash_sensor;
			crash_sensor.ChangeTrigger((gpio_num_t)data[0]).Setup().SetCallbackFunction(crash_sensor_callback);
			if(crash_notifier){
				crash_sensor.Disable();
				crash_notifier = false;
				return 1;
			}
			return 0;
		}
		case SENSOR_FLAME_DIG: {
			flame_sensor.SetupDigOuput((gpio_num_t) data[0]);
			return flame_sensor.GetDigValue();
		}
		case SENSOR_FLAME_AN: {
			flame_sensor.SetupAdcChannel((adc1_channel_t) data[0]);
			return flame_sensor.GetAnValue();
		}
		case SENSOR_HCSR04: {
			static HCSR04 distance_sensor;
			distance_sensor.Setup((gpio_num_t)data[0], (gpio_num_t)data[1]);
			int distance = distance_sensor.GetDistance();

			/* filtering garbage (maybe its only my sensor is broken) */
			return distance < 1e6 ? distance : -1;
		}
		case SENSOR_LINE_DIG: {
			static CLineDig line_digital;
			return line_digital.GetData((gpio_num_t)data[0]);
		}
		case SENSOR_SHARP: {
			static SHARP_2Y0A21 sharp_sensor;
			sharp_sensor.ChangePin(data[0]);
			return sharp_sensor.GetDistance();
		}

		default:
			return -0x30;
	}

	return 0;
}

inline void crash_sensor_callback(void*){
	crash_notifier = true;
}


inline bool is_pin_channel(int pin){
	int i = 0;
	while(i < 6 && ledc_channels[i] != -1 && ledc_channels[i] != pin){
		i++;
	}

	if (i >= 6 || ledc_channels[i] == -1) {
		return false;
	}

	return true;
}

inline ledc_channel_t pin_to_channel(int pin){
	if (ledc_channels[0] == -1){
		ledc_timer_config_t ledc_timer;

		ledc_timer.bit_num = LEDC_TIMER_13_BIT;
		ledc_timer.freq_hz = 5000;
		ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;
		ledc_timer.timer_num =  LEDC_TIMER_0;

		ledc_timer_config(&ledc_timer);
	}

	int i = 0;
	while(i < 6 && ledc_channels[i] != -1 && ledc_channels[i] != pin){
		i++;
	}

	if (i >= 6) {
		return LEDC_CHANNEL_0;
	}

	if (ledc_channels[i] == -1){
		ledc_channel_config_t ledc_channel;

		ledc_channel.channel = (ledc_channel_t)(i + 2);
		ledc_channel.duty = 0;
		ledc_channel.gpio_num = pin;
		ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
		ledc_channel.timer_sel =  LEDC_TIMER_0;

		ledc_channel_config(&ledc_channel);
		ledc_channels[i] = pin;
	}
	
	return (ledc_channel_t)(i + 2);
}

extern "C" void set_voltage(int pin, int level){
	if (is_pin_channel(pin) || (level > 0 && level < 255)){
		ledc_channel_t channel = pin_to_channel(pin);

		if (channel != LEDC_CHANNEL_0) {
			ledc_set_duty(LEDC_HIGH_SPEED_MODE, channel, 32*level);
			ledc_update_duty(LEDC_HIGH_SPEED_MODE, channel);
		}

		return;
	}

	gpio_num_t gpio = (gpio_num_t)pin;

	gpio_pad_select_gpio(gpio);
	gpio_set_direction(gpio, GPIO_MODE_OUTPUT);
	gpio_set_level(gpio, level <= 0 ? 0 : 1);
}

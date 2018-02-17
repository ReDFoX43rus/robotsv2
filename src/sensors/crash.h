#ifndef SENSORS_CRASH_H
#define SENSORS_CRASH_H

#include "driver/gpio.h"

class CrashSensor{
public:
	~CrashSensor();

	CrashSensor &ChangeTrigger(gpio_num_t trigger) {m_Trigger = trigger; return *this;}
	void Disable();
	CrashSensor &Setup();
	void SetCallbackFunction(gpio_isr_t Callback);

private:
	gpio_num_t m_Trigger;
};

#endif /* end of include guard: SENSORS_CRASH_H */

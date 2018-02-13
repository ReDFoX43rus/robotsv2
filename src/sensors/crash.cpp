#include "crash.h"
#include "esp_intr_alloc.h"
#include "stddef.h"

CrashSensor::CrashSensor(gpio_num_t trigger){
	m_Trigger = trigger;
}

CrashSensor::~CrashSensor(){
	Disable();
}

void CrashSensor::Disable(){
	gpio_intr_disable(m_Trigger);
	gpio_isr_handler_remove(m_Trigger);
	gpio_uninstall_isr_service();
}

CrashSensor &CrashSensor::Setup(){
	gpio_pad_select_gpio(m_Trigger);
	gpio_set_direction(m_Trigger, GPIO_MODE_INPUT);
	//gpio_set_pull_mode(m_Trigger, GPIO_PULLUP_ONLY);
	gpio_set_intr_type(m_Trigger, GPIO_INTR_POSEDGE);

	return *this;
}

void CrashSensor::SetCallbackFunction(gpio_isr_t Callback){
	gpio_intr_enable(m_Trigger);
	gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);
	gpio_isr_handler_add(m_Trigger, Callback, NULL);

}

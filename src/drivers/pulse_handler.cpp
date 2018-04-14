#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "time.h"

#include "pulse_handler.h"
#include "string.h"

CPulseHandler* CPulseHandler::s_apHandlers[PCNT_UNIT_MAX] = {0};

CPulseHandler::CPulseHandler(pcnt_unit_t pcnt_unit){
	m_Unit = pcnt_unit;
	memset((void*)&m_Configs, 0, sizeof(pcnt_config_t) * PCNT_CHANNEL_MAX);
	m_Paused = false;
}
CPulseHandler::~CPulseHandler(){
	Pause();
}

CPulseHandler &CPulseHandler::LoadTmpConfig(pcnt_channel_t channel){
	m_TmpConfig = m_Configs[channel];
	return *this;
}
CPulseHandler &CPulseHandler::ClearTmpConfig(){
	memset((void*)&m_TmpConfig, 0, sizeof(pcnt_config_t));
	m_TmpConfig.pulse_gpio_num = PCNT_PIN_NOT_USED;
	m_TmpConfig.ctrl_gpio_num = PCNT_PIN_NOT_USED;
	return *this;
}
CPulseHandler &CPulseHandler::SetPulseGPIO(gpio_num_t gpio){
	m_TmpConfig.pulse_gpio_num = (int)gpio;
	return *this;
}
CPulseHandler &CPulseHandler::SetControlGPIO(gpio_num_t gpio){
	m_TmpConfig.ctrl_gpio_num = (int)gpio;
	return *this;
}
CPulseHandler &CPulseHandler::SetLowControlMode(pcnt_ctrl_mode_t mode){
	m_TmpConfig.lctrl_mode = mode;
	return *this;
}
CPulseHandler &CPulseHandler::SetHighControlMode(pcnt_ctrl_mode_t mode){
	m_TmpConfig.hctrl_mode = mode;
	return *this;
}
CPulseHandler &CPulseHandler::SetPosMode(pcnt_count_mode_t mode){
	m_TmpConfig.pos_mode = mode;
	return *this;
}
CPulseHandler &CPulseHandler::SetNegMode(pcnt_count_mode_t mode){
	m_TmpConfig.neg_mode = mode;
	return *this;
}
CPulseHandler &CPulseHandler::SetHighLimit(int limit){
	m_TmpConfig.counter_h_lim = limit;
	return *this;
}
CPulseHandler &CPulseHandler::SetLowLimit(int limit){
	m_TmpConfig.counter_l_lim = limit;
	return *this;
}
bool CPulseHandler::ApplyConfig(pcnt_channel_t channel){
	m_TmpConfig.unit = m_Unit;
	m_TmpConfig.channel = channel;

	m_Configs[channel] = m_TmpConfig;

	return pcnt_unit_config(&m_TmpConfig) == ESP_OK;
}

bool CPulseHandler::Pause() {
	if(m_Paused)
		return false;

	pcnt_counter_pause(m_Unit);
	m_Paused = true;
	return true;
}
bool CPulseHandler::Resume() {
	if(!m_Paused)
		return false;

	pcnt_counter_resume(m_Unit);
	m_Paused = false;
	return true;
}
void CPulseHandler::Clear() {
	pcnt_counter_clear(m_Unit);
}

short CPulseHandler::GetValue(){
	short value;
	pcnt_get_counter_value(m_Unit, &value);
	return value;
}

uint32_t CPulseHandler::GetFrequency(uint32_t delay_ms){
	uint32_t ts1 = clock();
	short value1 = GetValue();
	bool wasPaused = Resume();

	vTaskDelay(pdMS_TO_TICKS(delay_ms));

	short value2 = GetValue();
	uint32_t ts2 = clock();

	if(wasPaused)
		Pause();

	uint32_t msDiff = (ts2 - ts1)*1000/CLOCKS_PER_SEC;
	if(msDiff == 0)
		return 0;

	return (value2 - value1) / msDiff;
}

bool CPulseHandler::FilterEnable(){
	return pcnt_filter_enable(m_Unit) == ESP_OK;
}
bool CPulseHandler::FilterDisable(){
	return pcnt_filter_disable(m_Unit) == ESP_OK;
}
bool CPulseHandler::SetFilterValue(uint16_t value){
	return pcnt_set_filter_value(m_Unit, value) == ESP_OK;
}
uint16_t CPulseHandler::GetFilterValue(){
	uint16_t value;
	pcnt_get_filter_value(m_Unit, &value);
	return value;
}

CPulseHandler* CPulseHandler::GetInstance(pcnt_unit_t pcnt_unit){
	if(pcnt_unit < 0 && pcnt_unit >= PCNT_UNIT_MAX)
		return NULL;

	if(s_apHandlers[pcnt_unit] == NULL){
		s_apHandlers[pcnt_unit] = new CPulseHandler(pcnt_unit);
		return s_apHandlers[pcnt_unit];
	}

	return s_apHandlers[pcnt_unit];
}

bool CPulseHandler::RemoveInstance(pcnt_unit_t pcnt_unit){
	if(pcnt_unit < 0 && pcnt_unit >= PCNT_UNIT_MAX)
		return false;
	
	if(s_apHandlers[pcnt_unit] == NULL)
		return false;

	delete s_apHandlers[pcnt_unit];
	s_apHandlers[pcnt_unit] = NULL;

	return true;
}

bool CPulseHandler::IsUnitFree(pcnt_unit_t pcnt_unit){
	return pcnt_unit > 0 && pcnt_unit < PCNT_UNIT_MAX && s_apHandlers[pcnt_unit] != NULL;
}
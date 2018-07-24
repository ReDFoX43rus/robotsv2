#include "lm35.h"

#include "unistd.h"
#include "adc.h"


LM35::LM35() {
	adc = new CAdc();
}

LM35::~LM35(){
	delete adc;
}

void LM35::ChangePin(int pin){
	if(pin == m_pin)
		return;

	adc->SetupAdc(pin, 1100);
	m_pin = pin;
}

int LM35::GetTemperature() {
	return (adc->GetVoltage(m_pin) / 4095.0) * 3.3 * 100;
}

#include "sharp_2y0a21.h"

#include "unistd.h"
#include "adc.h"


SHARP_2Y0A21::SHARP_2Y0A21() {
	adc = new CAdc();
}

SHARP_2Y0A21::~SHARP_2Y0A21(){
	delete adc;
}

void SHARP_2Y0A21::ChangePin(int pin){
	if(pin == m_pin)
		return;

	adc->SetupAdc(pin, 3000);
	m_pin = pin;
}

int SHARP_2Y0A21::GetDistance() {
	double power = pow(adc->GetVoltage(m_pin), -1.330678);
	uint32_t result = (uint32_t) (power * 933754.70664);
	if (result > 80)
	{
		return 80;
	}

	return result;
}

#include "sharp_2y0a21.h"

#include "unistd.h"
#include "adc.h"


SHARP_2Y0A21::SHARP_2Y0A21(int pin) {
	adc = new CAdc();
	adc->SetupAdc(pin, 3000);
	m_pin=pin;
}

SHARP_2Y0A21::~SHARP_2Y0A21(){
	delete adc;
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

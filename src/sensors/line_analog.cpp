#include "line_analog.h"

CLineAn::CLineAn(){
	m_Adc = new CAdc();
}

CLineAn::~CLineAn(){
	delete m_Adc;
}

int CLineAn::GetData(int pin){
	if(pin != m_Pin){
		m_Adc->SetupAdc(m_Pin, 1100);
		m_Pin = pin;
	}
	return m_Adc->GetVoltage(pin);
}

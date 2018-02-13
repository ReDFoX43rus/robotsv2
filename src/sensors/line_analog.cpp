#include "line_analog.h"

int CLineAn::GetData(){
	CAdc adc = CAdc();
	adc.SetupAdc(m_Pin, 1100);
	return adc.GetVoltage(m_Pin);
}

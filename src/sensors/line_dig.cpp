#include "line_dig.h"

CLineDig::CLineDig(gpio_num_t output):m_Output(output){
	gpio_pad_select_gpio(output);

	gpio_set_direction(output, GPIO_MODE_INPUT);
}

int CLineDig::GetData(){
	return gpio_get_level(m_Output);
}

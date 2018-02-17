#include "line_dig.h"

int CLineDig::GetData(gpio_num_t output){
	gpio_pad_select_gpio(output);

	gpio_set_direction(output, GPIO_MODE_INPUT);
	return gpio_get_level(m_Output);
}

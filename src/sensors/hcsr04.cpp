#include "hcsr04.h"

#include "unistd.h"
#include "esp_log.h"
#include "time.h"

static inline uint32_t timestamp(){
    uint32_t ccount;
    asm volatile ("rsr %0, ccount" : "=r"(ccount));
    return ccount / 16;
}

void HCSR04::Setup(gpio_num_t echo, gpio_num_t trig){
	m_Echo = echo;
	m_Trig = trig;

	gpio_pad_select_gpio(echo);
	gpio_pad_select_gpio(trig);

	gpio_set_direction(echo, GPIO_MODE_INPUT);
	gpio_set_direction(trig, GPIO_MODE_OUTPUT);
}

int HCSR04::GetDistance(){
	int timeout = 1000000/2;

	gpio_set_level(m_Trig, 1);
	usleep(10);
	gpio_set_level(m_Trig, 0);

	int level = gpio_get_level(m_Echo);
	int oldlevel = level;
	do{
		level = gpio_get_level(m_Echo);
	}while(level == oldlevel && --timeout);

	if(!timeout)
		return -1;

	uint32_t first = timestamp();
	timeout = 1000000/2;

	oldlevel = level;
	do{
		level = gpio_get_level(m_Echo);
	}while(level == oldlevel && --timeout);

	uint32_t second = timestamp();

	if(!timeout)
		return -1;

	return (second - first)/580;
}

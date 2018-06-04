#ifndef SERVO_SERVO
#define SERVO_SERVO

#include "driver/gpio.h"

class CServo{
public:
	CServo(){};
	CServo(gpio_num_t PWMA, gpio_num_t DIRA, gpio_num_t PWMB, gpio_num_t DIRB);

	enum direction_t {FORWARD, BACKWARD};
	enum side_t {LEFT_HAND, RIGHT_HAND};

	void Init();
	void Start(side_t side, direction_t direction, int power);
	void Stop();

private:
	bool m_Inited;

	gpio_num_t m_PWMA;
	gpio_num_t m_DIRA;
	gpio_num_t m_PWMB;
	gpio_num_t m_DIRB;

	int to_duty(int percent);

};



#endif

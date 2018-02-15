#ifndef SERVO_SERVO
#define SERVO_SERVO

#include "driver/gpio.h"

class CServo{
public:
	CServo(){};
	CServo(gpio_num_t IN1, gpio_num_t IN2, gpio_num_t IN3, gpio_num_t IN4, gpio_num_t ENA, gpio_num_t ENB);
	// typedef enum {FORWARD, BACKWARD} direction_t;
	// typedef enum {LEFT_HAND, RIGHT_HAND} side_t;

	enum direction_t {FORWARD, BACKWARD};
	enum side_t {LEFT_HAND, RIGHT_HAND};

	void Start(side_t side, direction_t direction, int power);
	void Stop();

private:
	bool m_Inited;

	gpio_num_t m_IN1;
	gpio_num_t m_IN2;
	gpio_num_t m_IN3;
	gpio_num_t m_IN4;
	gpio_num_t m_ENA;
	gpio_num_t m_ENB;

	int to_duty(int percent);

};



#endif

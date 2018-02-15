#include "servo.h"
#include "uart.h"

static CServo servo;
static int servo_inited = 0;

extern "C" int init_servo(gpio_num_t IN1, gpio_num_t IN2, gpio_num_t IN3, gpio_num_t IN4, gpio_num_t ENA, gpio_num_t ENB){
	if(servo_inited)
		return -1;

	servo = CServo(IN1, IN2, IN3, IN4, ENA, ENB);
	uart << "Servo inited" << endl;

	servo_inited++;
	return 0;
}

extern "C" int servo_power(int motor_n, int power){
	uart << "N: " << motor_n << " power: " << power << endl;

	if(!servo_inited)
		return -1;

	if(motor_n < 0 || motor_n > 1)
		return -1;

	if(!power){
		servo.Stop();
		return 0;
	}

	servo.Start(motor_n ? servo.RIGHT_HAND : servo.LEFT_HAND,
				servo.FORWARD, power);

	return 0;
}

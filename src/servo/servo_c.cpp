#include "servo.h"
#include "uart.h"

static CServo servo;
static int servo_inited = 0;

extern "C" int init_servo(gpio_num_t PWMA, gpio_num_t DIRA, gpio_num_t PWMB, gpio_num_t DIRB){
	if(servo_inited)
		return -1;

	servo = CServo(PWMA, DIRA, PWMB, DIRB);
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
				power > 0 ? servo.FORWARD : servo.BACKWARD, abs(power));

	return 0;
}

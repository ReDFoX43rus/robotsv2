#include "uart.h"
#include "cconsole/console.h"

#include "unistd.h"
#include "servo/servo.h"



void CmdServoHandler(int argc, char **argv){
	CServo c = CServo(GPIO_NUM_16, GPIO_NUM_17, GPIO_NUM_18, GPIO_NUM_19, GPIO_NUM_32, GPIO_NUM_33);
	if (atoi(argv[3])!=0){
		c.Stop();
	}
	c.Start(c.LEFT_HAND, c.FORWARD, atoi(argv[1]));
	c.Start(c.RIGHT_HAND, c.FORWARD, atoi(argv[2]));

}

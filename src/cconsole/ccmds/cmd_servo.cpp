#include "iobase/iobase.h"
#include "cconsole/console.h"

#include "unistd.h"
#include "servo/servo.h"



void CmdServoHandler(CIOBase &io, int argc, char **argv){
	CServo c = CServo(GPIO_NUM_22, GPIO_NUM_21, GPIO_NUM_17 /*GPIO_NUM_12*/, GPIO_NUM_13);
	if (atoi(argv[3])!=0){
		c.Stop();
	}
	c.Start(c.LEFT_HAND, c.FORWARD, atoi(argv[1]));
	c.Start(c.RIGHT_HAND, c.FORWARD, atoi(argv[2]));

}

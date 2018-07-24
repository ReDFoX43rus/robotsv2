/* Command handler for car's on-board computer */

#include "iobase/iobase.h"
#include "userspace/obc/controller.h"
#include "string.h"

static CController controller(GPIO_NUM_13, GPIO_NUM_14, GPIO_NUM_15, GPIO_NUM_16, GPIO_NUM_17, GPIO_NUM_18, GPIO_NUM_19, GPIO_NUM_12);

enum {
	CMD_OPEN_CAR,
	CMD_CLOSE_CAR,
	CMD_OPEN_WINDOWS,
	CMD_CLOSE_WINDOWS,
	CMD_OPEN_LEFT_DOOR,
	CMD_CLOSE_LEFT_DOOR,
	CMD_OPEN_RIGHT_DOOR,
	CMD_CLOSE_RIGHT_DOOR
};

void CmdObcHandler(CIOBase &io, int argc, char *argv[]){
	if(argc < 2){
		io << "Usage: " << argv[0] << " <cmd>" << endl;
		return;
	}

	char tmp = argv[1][0];
	uint8_t cmd = tmp - '0';

	switch(cmd){
		case CMD_OPEN_CAR:
			controller.OpenCar();
			break;
		case CMD_CLOSE_CAR:
			controller.CloseCar();
			break;
		case CMD_OPEN_WINDOWS:
			controller.OpenWindows();
			break;
		case CMD_CLOSE_WINDOWS:
			controller.CloseWindows();
			break;
		case CMD_OPEN_LEFT_DOOR:
			controller.OpenDoor(CController::LEFT);
			break;
		case CMD_CLOSE_LEFT_DOOR:
			controller.CloseDoor(CController::LEFT);
			break;
		case CMD_OPEN_RIGHT_DOOR:
			controller.OpenDoor(CController::RIGHT);
			break;
		case CMD_CLOSE_RIGHT_DOOR:
			controller.CloseDoor(CController::RIGHT);
			break;
		default:
			io << "Unknown cmd" << endl;
	}
}
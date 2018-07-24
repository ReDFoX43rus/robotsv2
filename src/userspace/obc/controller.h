/* This is the main logic for my obc */


#ifndef USERSPACE_OBC_CONTROLLER_H
#define USERSPACE_OBC_CONTROLLER_H

#include "driver/gpio.h"
#include "inttypes.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define DOOR_TRIGGER_DELAY_MS 50
#define WINDOW_TRIGGER_DELAY_MS (1000*6) // 6 seconds

class CController {
public:

	CController(gpio_num_t leftDoorClose, gpio_num_t rightDoorClose, gpio_num_t leftDoorOpen, gpio_num_t rightDoorOpen,
				gpio_num_t  leftWindowClose, gpio_num_t rightWindowClose, gpio_num_t leftWindowOpen, gpio_num_t rightWindowOpen);
	~CController();

	/* Since my car has only 2 doors
	 * I dont need specify front or rear door/window */

	enum ESIDE{
		LEFT,
		RIGHT
	};

	typedef ESIDE side_t;

	void OpenCar();
	void CloseCar();

	void OpenWindows();
	void CloseWindows();

	void OpenDoor(side_t side);
	void CloseDoor(side_t side);

private:
	/* It takes a lot of time to open/close window
	 * so we need a dedicated thread to do this */
	static void WindowTriggerTask(void *arg);

	/* Helping function for open/closing doors */
	static void TriggerPin(gpio_num_t pin, uint32_t delay_ms);

	/* Pins connected to doors' lock */
	gpio_num_t m_LeftDoorClosePin;
	gpio_num_t m_RightDoorClosePin;
	gpio_num_t m_LeftDoorOpenPin;
	gpio_num_t m_RightDoorOpenPin;

	gpio_num_t m_LeftWindowClosePin;
	gpio_num_t m_RightWindowClosePin;
	gpio_num_t m_LeftWindowOpenPin;
	gpio_num_t m_RightWindowOpenPin;
};

#endif /* USERSPACE_OBC_CONTROLLER_H */
/* This is the main logic for my obc */


#ifndef USERSPACE_OBC_CONTROLLER_H
#define USERSPACE_OBC_CONTROLLER_H

#include "driver/gpio.h"
#include "inttypes.h"

#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "drivers/nrf24l01p_lib.h"

#include "userspace/variables.h"

#define DOOR_TRIGGER_DELAY_MS 50
#define WINDOW_TRIGGER_DELAY_MS (1000*6) // 6 seconds

/* Spi-related pins */
#define SPI_SCLK 14
#define SPI_MISO 12
#define SPI_MOSI 13
#define SPI_CS GPIO_NUM_16

/* nRF24L01p-related pins */
#define NRF_PIN_IRQ GPIO_NUM_4
#define NRF_PIN_CE GPIO_NUM_17

/* Pins to interact with car */
#define OBC_PIN_DOOR_OPEN GPIO_NUM_18
#define OBC_PIN_DOOR_CLOSE GPIO_NUM_19
#define OBC_PIN_GND_RELAY GPIO_NUM_5

class CController {
public:

	CController(/* gpio_num_t leftDoorClose, gpio_num_t rightDoorClose, gpio_num_t leftDoorOpen, gpio_num_t rightDoorOpen,
				gpio_num_t  leftWindowClose, gpio_num_t rightWindowClose, gpio_num_t leftWindowOpen, gpio_num_t rightWindowOpen */);
	~CController();

	void SetupHSPI();
	void SetupNrf();

	/* Since my car has only 2 doors
	 * I dont need specify front or rear door/window */

	/* enum ESIDE{
		LEFT,
		RIGHT
	};

	typedef ESIDE side_t; */

	/* void OpenCar();
	void CloseCar();

	void OpenWindows();
	void CloseWindows();

	void OpenDoor(side_t side);
	void CloseDoor(side_t side); */

private:
	/* It takes a lot of time to open/close window
	 * so we need a dedicated thread to do this */
	static void WindowTriggerTask(void *arg);

	/* Helping function for open/closing doors */
	static void TriggerPin(gpio_num_t pin, uint32_t delay_ms);

	static void NrfIrqHandler(void *data);
	static void NRfIrqReadTask(void *data);

	CNRFLib *m_pNrf;

	void OpenDoors();
	void CloseDoors();

	void GroundRelayOff();
	void GroundRelayOn();

	/* Pins connected to doors' lock */
	// gpio_num_t m_LeftDoorClosePin;
	// gpio_num_t m_RightDoorClosePin;
	// gpio_num_t m_LeftDoorOpenPin;
	// gpio_num_t m_RightDoorOpenPin;

	// gpio_num_t m_LeftWindowClosePin;
	// gpio_num_t m_RightWindowClosePin;
	// gpio_num_t m_LeftWindowOpenPin;
	// gpio_num_t m_RightWindowOpenPin;
};

#endif /* USERSPACE_OBC_CONTROLLER_H */
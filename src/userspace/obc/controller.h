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
#define OBC_PIN_WINDOWS_CLOSE GPIO_NUM_27
#define OBC_PIN_GND_RELAY GPIO_NUM_5

typedef enum{
	ACS_TYPE_5A,
	ACS_TYPE_20A,
	ACS_TYPE_30A
} acs_type_t;

class CController {
public:

	CController();
	~CController();

	void SetupHSPI();
	void SetupNrf();
private:
	/* Helping function for open/closing doors */
	static void TriggerPin(gpio_num_t pin, uint32_t delay_ms);

	static void NrfIrqHandler(void *data);
	static void NRfIrqReadTask(void *data);

	static void HandleCmdTask(void *data);
	static TaskHandle_t m_hctTask;

	CNRFLib *m_pNrf;

	void OpenDoors();
	void CloseDoors();

	int MeasureCurrent(acs_type_t type);
};

typedef struct{
	CController* controller;
	controller_cmds_t cmd;	
} handle_cmd_data_t;

#endif /* USERSPACE_OBC_CONTROLLER_H */
#include "controller.h"
#include "uart.h"

#define DELAY_MS(x) vTaskDelay(pdMS_TO_TICKS((x)))

CController::CController(){
	m_pNrf = new CNRFLib(SPI_CS, NRF_PIN_CE);

	gpio_set_direction(OBC_PIN_DOOR_CLOSE, GPIO_MODE_OUTPUT);
	gpio_set_direction(OBC_PIN_DOOR_OPEN, GPIO_MODE_OUTPUT);
	gpio_set_direction(OBC_PIN_GND_RELAY, GPIO_MODE_OUTPUT);

	gpio_set_level(OBC_PIN_DOOR_CLOSE, 0);
	gpio_set_level(OBC_PIN_DOOR_OPEN, 0);
	gpio_set_level(OBC_PIN_GND_RELAY, 0);
}
CController::~CController(){
	delete m_pNrf;
	spi_bus_free(HSPI_HOST);
}

void CController::OpenDoors(){
	/* Turn off ground from native control panel */
	gpio_set_level(OBC_PIN_GND_RELAY, 1);
	DELAY_MS(5);

	gpio_set_level(OBC_PIN_DOOR_CLOSE, 0);
	gpio_set_level(OBC_PIN_DOOR_OPEN, 1);

	DELAY_MS(DOOR_TRIGGER_DELAY_MS);

	gpio_set_level(OBC_PIN_DOOR_OPEN, 0);

	/* Turn on ground again */
	gpio_set_level(OBC_PIN_GND_RELAY, 0);
}

void CController::CloseDoors(){
	/* Turn off ground from native control panel */
	gpio_set_level(OBC_PIN_GND_RELAY, 1);
	DELAY_MS(5);

	gpio_set_level(OBC_PIN_DOOR_OPEN, 0);
	gpio_set_level(OBC_PIN_DOOR_CLOSE, 1);

	DELAY_MS(DOOR_TRIGGER_DELAY_MS);

	gpio_set_level(OBC_PIN_DOOR_CLOSE, 0);

	/* Turn on ground again */
	gpio_set_level(OBC_PIN_GND_RELAY, 0);
}

void CController::SetupHSPI(){
	spi_bus_config_t buscfg;
	memset(&buscfg, 0, sizeof(buscfg));

	buscfg.miso_io_num = SPI_MISO;
	buscfg.mosi_io_num = SPI_MOSI;
	buscfg.sclk_io_num = SPI_SCLK;
	buscfg.quadhd_io_num = -1;
	buscfg.quadwp_io_num = -1;
	buscfg.max_transfer_sz = 4096;

	esp_err_t err = spi_bus_initialize(HSPI_HOST, &buscfg, 1);
	assert(err == ESP_OK);
}

void CController::SetupNrf(){
	assert(m_pNrf != NULL);

	m_pNrf->AttachToSpiBus(HSPI_HOST);
	m_pNrf->Begin(nrf_rx_mode);
	m_pNrf->SetPipeAddr(0, g_NrfAddr, 5);
	m_pNrf->AttachInterruptHandler(NRF_PIN_IRQ ,NrfIrqHandler, this);
}

void CController::NrfIrqHandler(void *data){
	if(data == NULL)
		return;

	xTaskCreatePinnedToCore(NRfIrqReadTask, "nrf_read_task", 4096, data, 4, NULL, 0);
}
void CController::NRfIrqReadTask(void *data){
	CController *controller = (CController*)data;
	CNRFLib *pNrf = controller->m_pNrf;

	nrf_reg_status_t status =  pNrf->GetStatus();
	if(status.rx_dr){
		uint8_t buffer[NRF_MAX_PAYLOAD] = {0};
		pNrf->Read(buffer, NRF_MAX_PAYLOAD);

		switch(buffer[0]){
			case CMD_CLOSE_DOORS:
				uart << "Closing doors" << endl;
				controller->CloseDoors();
				break;
			case CMD_OPEN_DOORS:
				uart << "Opening doors" << endl;
				controller->OpenDoors();
				break;
			case CMD_OPEN_WINDOWS:
				uart << "Opening windows" << endl;
				break;
		}
	}

	vTaskDelete(NULL);
}

void CController::GroundRelayOff(){

}
void CController::GroundRelayOn(){

}

/* void CController::OpenCar(){
	// OpenDoor(LEFT);
	// OpenDoor(RIGHT);
	// TriggerPin(m_LeftDoorOpenPin, DOOR_TRIGGER_DELAY_MS);
}

void CController::CloseCar(){
	// CloseDoor(LEFT);
	// CloseDoor(RIGHT);
	// TriggerPin(m_LeftDoorClosePin, DOOR_TRIGGER_DELAY_MS);
}

void CController::OpenWindows(){
	// xTaskCreate(WindowTriggerTask, "window_trigger_task_open_left", 1024, (void*)m_LeftDoorOpenPin, 4, NULL);
	// xTaskCreate(WindowTriggerTask, "window_trigger_task_open_right", 1024, (void*)m_RightDoorOpenPin, 4, NULL);
}

void CController::CloseWindows(){
	// xTaskCreate(WindowTriggerTask, "window_trigger_task_close_left", 1024, (void*)m_LeftDoorClosePin, 4, NULL);
	// xTaskCreate(WindowTriggerTask, "window_trigger_task_close_right", 1024, (void*)m_RightDoorClosePin, 4, NULL);
} 

void CController::WindowTriggerTask(void *arg){
	int pin = (int)arg;

	TriggerPin((gpio_num_t)pin, WINDOW_TRIGGER_DELAY_MS);
	vTaskDelete(NULL);
}

void CController::OpenDoor(side_t side){
	// gpio_num_t pin = side == LEFT ? m_LeftDoorOpenPin : m_RightDoorOpenPin;
	// TriggerPin(pin, DOOR_TRIGGER_DELAY_MS);
}

void CController::CloseDoor(side_t side){
	// gpio_num_t pin = side == LEFT ? m_LeftDoorClosePin : m_RightDoorClosePin;
	// TriggerPin(pin, DOOR_TRIGGER_DELAY_MS);
}
*/

void CController::TriggerPin(gpio_num_t pin, uint32_t delay_ms){
	gpio_pad_select_gpio(pin);
	gpio_set_direction(pin, GPIO_MODE_OUTPUT);

	gpio_set_level(pin, 1);
	vTaskDelay(pdMS_TO_TICKS(delay_ms));
	gpio_set_level(pin, 0);	
}

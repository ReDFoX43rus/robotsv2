#include "controller.h"
#include "uart.h"
#include "time.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define DEFAULT_VREF 1092
#define ACS712_SUPPLY_VOLTAGE 3300
#define DELAY_MS(x) vTaskDelay(pdMS_TO_TICKS((x)))

static handle_cmd_data_t hctData;

TaskHandle_t CController::m_hctTask = NULL;

CController::CController(){
	m_pNrf = new CNRFLib(SPI_CS, NRF_PIN_CE);

	gpio_set_direction(OBC_PIN_DOOR_CLOSE, GPIO_MODE_OUTPUT);
	gpio_set_direction(OBC_PIN_DOOR_OPEN, GPIO_MODE_OUTPUT);
	gpio_set_direction(OBC_PIN_GND_RELAY, GPIO_MODE_OUTPUT);
	gpio_set_direction(OBC_PIN_WINDOWS_CLOSE, GPIO_MODE_OUTPUT);

	gpio_set_level(OBC_PIN_DOOR_CLOSE, 0);
	gpio_set_level(OBC_PIN_DOOR_OPEN, 0);
	gpio_set_level(OBC_PIN_GND_RELAY, 0);
	gpio_set_level(OBC_PIN_WINDOWS_CLOSE, 0);
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
	gpio_set_level(OBC_PIN_WINDOWS_CLOSE, 1);

	DELAY_MS(DOOR_TRIGGER_DELAY_MS);

	gpio_set_level(OBC_PIN_DOOR_CLOSE, 0);

	gpio_set_level(OBC_PIN_WINDOWS_CLOSE, 1);

	clock_t ts1 = clock() / CLOCKS_PER_SEC;
	clock_t ts2 = ts1;
	uint32_t timeout = 6 * 1000;
	int current;
	do{
		DELAY_MS(200);
		ts2 = clock() / CLOCKS_PER_SEC;
		current = MeasureCurrent(ACS_TYPE_20A);
	}while(abs(current) < 10 && ts2 - ts1 < timeout);

	gpio_set_level(OBC_PIN_WINDOWS_CLOSE, 0);

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
	m_pNrf->SetTxAddr(g_NrfAddr, 5);
	m_pNrf->SetPipeAddr(0, g_NrfAddr, 5);
	m_pNrf->AttachInterruptHandler(NRF_PIN_IRQ ,NrfIrqHandler, this);
}

void CController::NrfIrqHandler(void *data){
	if(data == NULL)
		return;

	xTaskCreatePinnedToCore(NRfIrqReadTask, "nrf_read_task", 4096, data, 4, NULL, 0);
}
void CController::NRfIrqReadTask(void *data){
	static uint8_t expectedValue = 0;

	CController *controller = (CController*)data;
	CNRFLib *pNrf = controller->m_pNrf;

	nrf_reg_status_t status =  pNrf->GetStatus();
	if(status.rx_dr){
		uint8_t buffer[NRF_MAX_PAYLOAD] = {0};
		pNrf->Read(buffer, NRF_MAX_PAYLOAD);

		if(buffer[0] == CMD_REQUEST_VALUE){
			srand(clock());
			for(int i = 0; i < NRF_MAX_PAYLOAD; i++)
				buffer[i] = rand() & 0xFF;

			expectedValue = 0;
			for(int i = 1; i < NRF_MAX_PAYLOAD; i++)
				expectedValue += (~buffer[i]) & buffer[i-1];

			pNrf->SetSleepTxMode();
			pNrf->Send(buffer, NRF_MAX_PAYLOAD);
			pNrf->SetRxMode();

			vTaskDelete(NULL);
		}

		if(buffer[1] != expectedValue){
			vTaskDelete(NULL);
		}

		if(m_hctTask != NULL){
			vTaskDelete(m_hctTask);
			m_hctTask = NULL;
		}

		hctData.controller = controller;
		hctData.cmd = (controller_cmds_t)buffer[0];
		xTaskCreatePinnedToCore(HandleCmdTask, "handle_cmd", 512, &hctData, 4, &m_hctTask, 0);
	}

	vTaskDelete(NULL);
}

void CController::HandleCmdTask(void *data){
	handle_cmd_data_t *tmp = (handle_cmd_data_t*)data;
	CController *controller = tmp->controller;
	controller_cmds_t cmd = tmp->cmd;

	switch(cmd){
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
		default:
			break;
	}

	vTaskSuspend(NULL);
}

void CController::TriggerPin(gpio_num_t pin, uint32_t delay_ms){
	gpio_pad_select_gpio(pin);
	gpio_set_direction(pin, GPIO_MODE_OUTPUT);

	gpio_set_level(pin, 1);
	vTaskDelay(pdMS_TO_TICKS(delay_ms));
	gpio_set_level(pin, 0);	
}

int CController::MeasureCurrent(acs_type_t type){
	esp_adc_cal_characteristics_t adc_chars;
	esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, DEFAULT_VREF, &adc_chars);

	int reading =  adc1_get_raw(ADC1_CHANNEL_4);
	int voltage = esp_adc_cal_raw_to_voltage(reading, &adc_chars);
	
	int center = ACS712_SUPPLY_VOLTAGE / 2;
	int diff = voltage - center;

	int sens;
	switch(type){
		case ACS_TYPE_5A:
			sens = 185;
			break;
		case ACS_TYPE_20A:
			sens = 100;
			break;
		case ACS_TYPE_30A:
			sens = 66;
			break;
		default:
			return 0;
	}

	return diff/sens;
}
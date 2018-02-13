#include "uart.h"
#include <cstring>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

CUart uart = CUart(GPIO_NUM_1, GPIO_NUM_3, UART_NUM_0);

CUart::CUart(int txd, int rxd, uart_port_t uartNum, size_t uartBuffSize){
	m_Pinout = {
		.txd = txd,
		.rxd = rxd,
		.rts = UART_PIN_NO_CHANGE,
		.cts = UART_PIN_NO_CHANGE
	};

	m_Config = {
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.parity    = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE
	};

	m_UartNum = uartNum;
	m_BuffSize = uartBuffSize;

	uart_param_config(uartNum, &m_Config);
	uart_set_pin(uartNum, txd, rxd, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
	uart_disable_pattern_det_intr(uartNum);
	uart_driver_install(uartNum, uartBuffSize, 0, 0, NULL, 0);
}

CUart::~CUart(){}

uint32_t CUart::GetChar(){
	uint8_t result = 0;

	while(1){
		int len = GetBufferedDataLength();
		if(len > 0){
			uart_read_bytes(m_UartNum, &result, 1, 0);
			break;
		}

		// Need to feed watchdog ?
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}

	return (char)result;
}

size_t CUart::Write(const char *data, size_t size){
	return uart_write_bytes(m_UartNum, data, size);
}

bool CUart::Flush(){
	esp_err_t err = uart_flush(m_UartNum);
	return (err == ESP_OK);
}

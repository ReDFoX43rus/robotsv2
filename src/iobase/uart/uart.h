#ifndef UART_CUART_H
#define UART_CUART_H

#include "driver/uart.h"
#include "../iobase.h"

typedef struct uart_pinout {
	int txd;
	int rxd;
	int rts;
	int cts;
} uart_pinout_t;

class CUart : public CIOBase {
public:
	// Default uart setup
	CUart(int txd, int rxd, uart_port_t uartNum, size_t uartBuffSize = 1024);
	~CUart();

	inline int GetBytes(char *data, size_t size){
		return uart_read_bytes(m_UartNum, (uint8_t *)data, size, 0);
	}
	uint32_t GetChar();
	// Writes data to uart
	size_t Write(const char *data, size_t size);

	// Size of RXD buffer
	inline int GetBufferedDataLength(){
		size_t length;
		esp_err_t err = uart_get_buffered_data_len(m_UartNum, &length);
		return err == ESP_OK ? length : -length;
	}
	// Flush RXD buffer
	bool Flush();
private:
	uart_config_t m_Config;
	uart_pinout_t m_Pinout;
	uart_port_t m_UartNum;
	size_t m_BuffSize;
};

extern CUart uart;

#endif /* end of include guard: UART_CUART_H */

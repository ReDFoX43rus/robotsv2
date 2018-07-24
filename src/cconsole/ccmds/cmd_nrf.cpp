#include "iobase/iobase.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// #include "drivers/rf24.h"
// #include "drivers/nrf24l01.h"
// #include "driver/spi_master.h"
// #include <util/spi_builder.h>

// #define SPI_SCLK 14
// #define SPI_MISO 12
// #define SPI_MOSI 13

// static uint8_t addr[] = {0x1, 0x2, 0x3, 0x4, 0x5};

// static RF24 nrf(GPIO_NUM_17, GPIO_NUM_16);
// const uint64_t pipe = 0xDEADBEEF;

#include "drivers/nrf24l01p_lib.h"
#include <string.h>

#define SPI_SCLK 14
#define SPI_MISO 12
#define SPI_MOSI 13

void CmdNrfHandler(CIOBase &io, int argc, char *argv[]){
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

	CNRFLib nrf(GPIO_NUM_16, GPIO_NUM_17);
	nrf.AttachToSpiBus(HSPI_HOST);
	
	uint8_t buff[32] = {0};
	uint8_t addr[5] = {255, 244, 233, 222, 211};

	if(argc == 1){
		io << "Tx mode" << endl;

		for(int i = 0; i < 32; i++){
			buff[i] = i;
		}

		nrf.SetTxAddr(addr, 5);
		nrf.Begin(nrf_tx_mode);

		int8_t result;
		while(1){
			result = nrf.Send(buff, 32);
			buff[1]++;

			io << "Result: " << result << endl;
			vTaskDelay(pdMS_TO_TICKS(500));
		}
	} else {
		io << "Rx mode" << endl;

		nrf.SetPipeAddr(0, addr, 5);
		nrf.Begin(nrf_rx_mode);

		while(1){
			vTaskDelay(pdMS_TO_TICKS(1));

			if(!nrf.IsRxDataAvailable())
				continue;

			nrf.Read(buff, 32);
			io << "Received: ";
			for(int i = 0; i < 32; i++){
				io << buff[i] << " ";
			}
			io << endl;
		}
	}

	/* uint8_t pipe = 0;
	uint8_t buff[32] = {0};

	NRF24L01 nrf(0, GPIO_NUM_16, GPIO_NUM_17);
	nrf.RxPipeSetup(pipe, addr, 32);

	if(argc == 1){
		io << "Tx mode" << endl;
		while(1){
			int r = rand() & 0xFF;
			buff[0] = r;
			io << "Sending " << r;
			int8_t result = nrf.Send(addr, buff, 32);
			io << " Result: " << result << endl;

			vTaskDelay(pdMS_TO_TICKS(500));
		}
	} else {
		io << "Recv mode" << endl;
		while(1){
			uint8_t len = nrf.Recv(&pipe, buff);
			io << "Received " << len << " bytes, payload: " << buff[0] << endl;

			vTaskDelay(pdMS_TO_TICKS(500));
		}
	} */

	/* nrf.begin();
	nrf.setRetries(15, 15);
	nrf.setAutoAck(1);
	nrf.setChannel(70);
	nrf.setPALevel(RF24_PA_MAX);
	nrf.setDataRate(RF24_1MBPS);
	if(argc == 1){
		io << "Tx mode" << endl;

		nrf.openWritingPipe(pipe);
		nrf.startListening();
		uint8_t r;
		bool result;
		while(1){
			r = rand() & 0xFF;
			result = nrf.write(&r, 1);

			io << "Sent: " << r << " result: " << result << endl;

			vTaskDelay(pdMS_TO_TICKS(500));
		}
	} else {
		io << "Rx mode" << endl;

		nrf.openReadingPipe(1, pipe);
		nrf.startListening();
		uint8_t recv = 0;
		while(1){
			vTaskDelay(pdMS_TO_TICKS(500));

			if(!nrf.available())
				continue;

			nrf.read(&recv, 1);
			io << "Received: " << recv << endl;		}
	} */
}
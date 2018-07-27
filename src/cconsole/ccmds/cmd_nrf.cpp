#include "iobase/iobase.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "drivers/nrf24l01p_lib.h"
#include <string.h>

#define SPI_SCLK 14
#define SPI_MISO 12
#define SPI_MOSI 13

#include "uart.h"

static void ReadTask(void *data){
	if(data == NULL)
		return;

	uart << "Reading rx" << endl;
	CNRFLib *pNrf = (CNRFLib*)data;
	nrf_reg_status_t status =  pNrf->GetStatus();
	if(status.rx_dr){
		uint8_t buffer[32] = {0};
		pNrf->Read(buffer, 32);

		uart << "Received: ";
		for(int i = 0; i < 32; i++)
			uart << buffer[i] << " ";
		uart << endl;
	}

	vTaskDelete(NULL);
}

static void IrqHandler(void* data){
	uart << "Handling interrupt" << endl;
	
	xTaskCreatePinnedToCore(ReadTask, "nrf_read_task", 4096, data, 4, NULL, 0);
}

CNRFLib nrf(GPIO_NUM_16, GPIO_NUM_17);

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

	nrf.AttachToSpiBus(HSPI_HOST);
	
	uint8_t buff[32] = {0};
	uint8_t addr[5] = {222, 111, 001, 100, 040};

	if(argc == 1){
		io << "Tx mode" << endl;

		for(int i = 0; i < 32; i++){
			buff[i] = i;
		}

		nrf.Begin(nrf_tx_mode);
		nrf.SetTxAddr(addr, 5);
		nrf.SetPipeAddr(0, addr, 5);

		/* uint64_t channels1, channels2;
		nrf.ScanChannels(channels1, channels2);

		uart << "Channels result:" << endl;
		for(int i = 0; i < 64; i++)
			uart << "#" << i << " : " << (uint8_t)((channels1 >> i) & 1) << endl;

		for(int i = 0; i < 64; i++)
			uart << "#" << i+64 << " : " << (uint8_t)((channels2 >> i) & 1) << endl;

		nrf.SetSleepTxMode(); */

		int8_t result;
		while(1){
			result = nrf.Send(buff, 32);
			buff[1]++;

			io << "Result: " << result << endl;
			vTaskDelay(pdMS_TO_TICKS(500));
		}
	} else {
		io << "Rx mode" << endl;

		nrf.Begin(nrf_rx_mode);
		nrf.SetPipeAddr(0, addr, 5);

		memset(addr, 0, 5 * sizeof(uint8_t));
		nrf.GetPipeAddr(0, addr);
		io << "Pipe addr: ";
		for(int i = 0; i < 5; i++)
			io << addr[i] << " ";
		io << endl;

		nrf.AttachInterruptHandler(GPIO_NUM_4, IrqHandler);


		/* while(1){
			vTaskDelay(pdMS_TO_TICKS(1));

			if(!nrf.IsRxDataAvailable())
				continue;

			nrf.Read(buff, 32);
			io << "Received: ";
			for(int i = 0; i < 32; i++){
				io << buff[i] << " ";
			}
			io << endl;
		} */
	}
}
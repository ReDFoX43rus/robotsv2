/* Command handler for car's on-board computer */

#include "iobase/iobase.h"
#include "userspace/obc/controller.h"
#include "userspace/manager/manager.h"
#include "string.h"

static CController controller;
// static bool managerInited = false;
static CManager manager(GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_27);
static bool nrfInited = false;
static CNRFLib nrf(GPIO_NUM_16, GPIO_NUM_17);
void CmdObcHandler(CIOBase &io, int argc, char *argv[]){

	if(argc == 1){
		controller.SetupHSPI();
		controller.SetupNrf();
		io << "Controller installed" << endl;
		return;
	}

	/* if(!managerInited){
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

		manager.SetupNrf(HSPI_HOST, GPIO_NUM_16, GPIO_NUM_17);
		manager.SetupButtons();
		managerInited = true;

		io << "Manager inited" << endl;
		return;
	}

	io << "Manager already inited" << endl; */

	if(!nrfInited){	
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
		nrf.Begin(nrf_tx_mode);
		nrf.SetTxAddr(g_NrfAddr, 5);
		nrf.SetPipeAddr(0, g_NrfAddr, 5);

		nrfInited = true;
	}

	char *cmd = argv[1];
	uint8_t buffer[NRF_MAX_PAYLOAD] = {0};

	if(!strcmp(cmd, "open")){
		buffer[0] = CMD_OPEN_DOORS;
		nrf.Send(buffer, NRF_MAX_PAYLOAD);
	} else if(!strcmp(cmd, "close")){
		buffer[0] = CMD_CLOSE_DOORS;
		nrf.Send(buffer, NRF_MAX_PAYLOAD);
	}
}
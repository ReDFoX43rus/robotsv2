#include "sdcard.h"
#include "../iobase/uart/uart.h"

#include "esp_err.h"
#include "esp_vfs_fat.h"

CSDCard::CSDCard(int miso, int mosi, int clk, int cs, int wp, size_t maxFiles){

	m_Miso = (gpio_num_t)miso;
	m_Mosi = (gpio_num_t)mosi;
	m_Clk = (gpio_num_t)clk;
	m_Cs = (gpio_num_t)cs;
	m_Wp = (gpio_num_t)wp;
	m_MaxFiles = maxFiles;

	m_Host = SDSPI_HOST_DEFAULT();
	m_SlotConfig = SDSPI_SLOT_CONFIG_DEFAULT();
	m_SlotConfig.gpio_miso	= m_Miso;
	m_SlotConfig.gpio_mosi	= m_Mosi;
	m_SlotConfig.gpio_sck	= m_Clk;
	m_SlotConfig.gpio_cs	= m_Cs;
	m_SlotConfig.gpio_wp	= m_Wp;
}

bool CSDCard::MountFat(const char *path, bool formatOnFail){
	esp_vfs_fat_sdmmc_mount_config_t mount_config = {
		.format_if_mount_failed = formatOnFail,
		.max_files = (int)m_MaxFiles
	};

	sdmmc_card_t* card;
	esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &m_Host, &m_SlotConfig, &mount_config, &card);

	if (ret != ESP_OK) {
		uart << "An error occurred while mounting fatfs to sdcard: " << ret << endl;
		switch(ret){
			case ESP_ERR_NO_MEM:
				uart << "Not enough memory, check LOGD" << endl;
				break;
			case ESP_FAIL:
				uart << "Failed to mount filesystem" << endl;
				break;
			default:
				uart << "Failed to initialize the card" << endl
				<< "Make sure SD card line have pull-up resistors in place" << endl;
		};
		return false;
	}
	return true;
}

#include "sdcard.h"
#include "../iobase/uart/uart.h"

#include "esp_err.h"
#include "esp_vfs_fat.h"

CSDCard::CSDCard(int miso, int mosi, int clk, int cs, int wp, size_t maxFiles){

	/* m_Miso = (gpio_num_t)miso;
	m_Mosi = (gpio_num_t)mosi;
	m_Clk = (gpio_num_t)clk;
	m_Cs = (gpio_num_t)cs;
	m_Wp = (gpio_num_t)wp;
	m_MaxFiles = maxFiles;

	m_Host = {SDMMC_HOST_FLAG_SPI, HSPI_HOST, SDMMC_FREQ_DEFAULT, 3.3f, sdspi_host_init, NULL, NULL, sdspi_host_set_card_clk, sdspi_host_do_transaction, sdspi_host_deinit, NULL, NULL, 0};
	m_SlotConfig.gpio_miso	= GPIO_NUM_2;
	m_SlotConfig.gpio_mosi	= GPIO_NUM_15;
	m_SlotConfig.gpio_sck	= GPIO_NUM_14;
	m_SlotConfig.gpio_cs	= GPIO_NUM_13;
	m_SlotConfig.gpio_wp	= SDSPI_SLOT_NO_WP;
	m_SlotConfig.gpio_cd 	= SDSPI_SLOT_NO_CD; */
/* 	m_SlotConfig = {\
    .gpio_miso = GPIO_NUM_2, \
    .gpio_mosi = GPIO_NUM_15, \
    .gpio_sck  = GPIO_NUM_14, \
    .gpio_cs   = GPIO_NUM_13, \
    .gpio_cd   = SDSPI_SLOT_NO_CD, \
    .gpio_wp   = SDSPI_SLOT_NO_WP, \
    .dma_channel = 1 \
} */

	const char *TAG = "SDCARD";
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.

    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 16,
        .allocation_unit_size = 16 * 1024
    };

    // Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc_mount is an all-in-one convenience function.
    // Please check its source code and implement error recovery when developing
    // production applications.
    sdmmc_card_t* card;
    esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                "If you want the card to be formatted, set format_if_mount_failed = true.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return;
    }

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);
}

bool CSDCard::MountFat(const char *path, bool formatOnFail){
	esp_vfs_fat_sdmmc_mount_config_t mount_config = {
		.format_if_mount_failed = formatOnFail,
		.max_files = (int)m_MaxFiles,
		.allocation_unit_size = 16 * 1024
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

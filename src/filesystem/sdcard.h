#ifndef FILESYSTEM_SDCARD_H
#define FILESYSTEM_SDCARD_H

#include "driver/gpio.h"

#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"

class CSDCard{
public:
	CSDCard(int miso, int mosi, int clk, int cs, int wp, size_t maxFiles);
	~CSDCard(){}

	bool MountFat(const char *path, bool formatOnFail = true);
private:
	sdmmc_host_t m_Host;
	sdspi_slot_config_t m_SlotConfig;
	gpio_num_t m_Miso;
	gpio_num_t m_Mosi;
	gpio_num_t m_Clk;
	gpio_num_t m_Cs;
	gpio_num_t m_Wp;
	size_t m_MaxFiles;
};

#endif /* end of include guard: FILESYSTEM_SDCARD_H */

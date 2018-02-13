#include "esp_log.h"
#include "esp_vfs_dev.h"
#include "esp_vfs_fat.h"

#define MOUNT_PATH "/fat"

static const char *TAG = "[filesystem]";

void initialize_filesystem(void)
{
	static wl_handle_t wl_handle;
	const esp_vfs_fat_mount_config_t mount_config = {
		.max_files = 16,
		.format_if_mount_failed = true
	};
	esp_err_t err = esp_vfs_fat_spiflash_mount(MOUNT_PATH, "storage", &mount_config, &wl_handle);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to mount FATFS (0x%x)", err);
		return;
	}
}

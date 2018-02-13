#include <inttypes.h>
#include "esp_log.h"
#include "sdkconfig.h"

#define IRAM_LENGTH 0x20000
#define DRAM_LENGTH (0x50000 - CONFIG_TRACEMEM_RESERVE_DRAM - CONFIG_BT_RESERVE_DRAM)

extern int _heap_start;
extern int _heap_end;
extern int _data_start;
extern int _data_end;
extern int _bss_start;
extern int _bss_end;
extern int _iram_text_start;
extern int _iram_text_end;

static const char *TAG = "[memcheck]";

void print_mem_usage(void){
	uint32_t total_iram = _iram_text_end - _iram_text_start;
	ESP_LOGI(TAG, "IRAM TEXT: start: 0x%8X end: 0x%8X total: %u", _iram_text_start, _iram_text_end, total_iram);

	uint32_t total_data = _data_end - _data_start;
	ESP_LOGI(TAG, "DATA: start: 0x%8X end: 0x%8X total: %u", _data_start, _data_end, total_data);

	uint32_t total_bss = _bss_end - _bss_start;
	ESP_LOGI(TAG, "BSS: start: 0x%8X end: 0x%8X total: %u", _bss_start, _bss_end, total_bss);

	uint32_t total_heap = _heap_end - _heap_start;
	ESP_LOGI(TAG, "HEAP: start: 0x%8X end: 0x%8X total: %u", _heap_start, _heap_end, total_heap);

	uint32_t total_dram = total_data + total_bss;
	ESP_LOGI(TAG, "DRAM: used %u%% (%u/%u), free: %u", (100*total_dram) / DRAM_LENGTH, total_dram, DRAM_LENGTH, DRAM_LENGTH - total_dram);
	ESP_LOGI(TAG, "IRAM: used %u%% (%u/%u), free: %u", (100*total_iram) / IRAM_LENGTH, total_iram, IRAM_LENGTH, IRAM_LENGTH - total_iram);
}

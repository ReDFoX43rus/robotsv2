#include "iobase/iobase.h"

#include "esp_ota_ops.h"
#include "esp_system.h"

#define UPDATE_BUFF_SIZE 1024

void CmdUpdateHandler(CIOBase &io, int argc, char *argv[]){
	if(argc != 2){
		io << "Usage: " << argv[0] << " firmware_size" << endl;
		return;
	}

	int ota_size = atoi(argv[1]);
	esp_ota_handle_t update_handle = 0 ;

	const esp_partition_t *configured = esp_ota_get_boot_partition();
	const esp_partition_t *running = esp_ota_get_running_partition();

	if(configured != running)
		io << "Configured OTA partition at " << configured->address << " but running from " << running->address << endl;

	io << "Running partition at " << running->address << " type: " << running->type << " subtype: " << running->subtype << endl;

	const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);
	assert(update_partition != NULL);

	io << "Updating to addr: " << update_partition->address << " subtype: " << update_partition->subtype << endl;

	esp_err_t err = esp_ota_begin(update_partition, ota_size, &update_handle);
	if (err != ESP_OK) {
		io << "esp_ota_begin failed, error=" << err << endl;
		return;
	}

	int current = 0, buffSize, actual, percent = 0, last_percent = 0;
	char buff[UPDATE_BUFF_SIZE];

	while(current < ota_size){
		buffSize = io.GetBufferedDataLength();
		if(buffSize <= 0)
			continue;

		if(buffSize > UPDATE_BUFF_SIZE)
			buffSize = UPDATE_BUFF_SIZE;

		actual = io.GetBytes(buff, buffSize);
		if(!actual){
			io << "Buffered bytes cannot be read, terminating..." << endl;
			esp_ota_end(update_handle);
			return;
		}

		if((err = esp_ota_write(update_handle, (const void *)buff, actual)) != ESP_OK){
			io << "esp_ota_write error (" << err << "), terminating..." << endl;
			esp_ota_end(update_handle);
			return;
		}

		current += actual;
		percent = current * 100 / ota_size;

		if(percent != last_percent){
			io << "Progress: " << percent << "%" << endl;
			last_percent = percent;
		}
	}

	if((err = esp_ota_end(update_handle)) != ESP_OK){
		io << "esp_ota_end error (" << err << ")" << endl;
		return;
	}

	if((err = esp_ota_set_boot_partition(update_partition)) != ESP_OK){
		io << "Cannot set new boot partition (" << err << ")" << endl;
		return;
	}

	io << "Done! Rebooting..." << endl;
	esp_restart();
}

#include <iobase/iobase.h>
#include <fileio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"

static const i2s_port_t i2s_num = (i2s_port_t)0; // i2s port number

typedef struct {
	uint16_t wFormatTag;	  // Формат звуковых данных
	uint16_t nChannels;		  // Количество каналов
	uint32_t nSamplesPerSec;  // Частота дискретизации аудиосигнала
	uint32_t nAvgBytesPerSec; // Количество байт передаваемых в секунду
	uint16_t nBlockAlign;	 // Выравнивание данных в чанке данных

	uint16_t vBitsPerSample; //Количество бит на одну выборку сигнала
} wav_t;

void CmdSoundHandler(CIOBase &io, int argc, char *argv[]){
	const char *filename = argv[1];

	CFile file(filename, FM_READ);

	if(!file.IsOpened()){
		io << "Cannot open file" << endl;
		return;
	}

	wav_t wav;
	if(file.GetString((char*)&wav, sizeof(wav)) <= 0){
		io << "Error reading wav header" << endl;
		return;
	}

	io << "FormatTag: " << wav.wFormatTag << " Channels: " << wav.nChannels << " SampleRate: " << wav.nSamplesPerSec 
		<< " AvgBytesPerSec: " << wav.nAvgBytesPerSec << " BlockAlign: " << wav.nBlockAlign << endl;

	static const i2s_config_t i2s_config = {
		.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
		.sample_rate = 44100,
		.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, /* the DAC module will only take the 8bits from MSB */
		.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
		.communication_format = I2S_COMM_FORMAT_I2S_MSB,
		.intr_alloc_flags = 0, // default interrupt priority
		.dma_buf_count = 8,
		.dma_buf_len = 64,
		.use_apll = false
	};

	i2s_driver_install(i2s_num, &i2s_config, 0, NULL); //install and start i2s driver

	i2s_set_pin(i2s_num, NULL); //for internal DAC, this will enable both of the internal channels

	//You can call i2s_set_dac_mode to set built-in DAC output mode.
	//i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);

	i2s_set_sample_rates(i2s_num, wav.nSamplesPerSec); //set sample rates

	i2s_zero_dma_buffer(i2s_num);

	char buffer[64];
	uint8_t len;
	size_t bytesWritten, totalInChunk;
	while((len = file.GetString(buffer, 64)) > 0){
		totalInChunk = 0;

		do{
			i2s_write(i2s_num, (void*)buffer, len, &bytesWritten, pdMS_TO_TICKS(100));
			totalInChunk += bytesWritten;

			if(totalInChunk < len)
				vTaskDelay(pdMS_TO_TICKS(100));
				
		} while(totalInChunk < len);
	}	

	i2s_driver_uninstall(i2s_num); //stop & destroy i2s driver
	file.Close();
}
#include <iobase/iobase.h>
#include <fileio.h>
#include <freertos/FreeRTOS.h>
#include "driver/i2s.h"

#include "math.h"

struct WAVHEADER
{
	// WAV-формат начинается с RIFF-заголовка:

	// Содержит символы "RIFF" в ASCII кодировке
	// (0x52494646 в big-endian представлении)
	char chunkId[4];

	// 36 + subchunk2Size, или более точно:
	// 4 + (8 + subchunk1Size) + (8 + subchunk2Size)
	// Это оставшийся размер цепочки, начиная с этой позиции.
	// Иначе говоря, это размер файла - 8, то есть,
	// исключены поля chunkId и chunkSize.
	unsigned long chunkSize;

	// Содержит символы "WAVE"
	// (0x57415645 в big-endian представлении)
	char format[4];

	// Формат "WAVE" состоит из двух подцепочек: "fmt " и "data":
	// Подцепочка "fmt " описывает формат звуковых данных:

	// Содержит символы "fmt "
	// (0x666d7420 в big-endian представлении)
	char subchunk1Id[4];

	// 16 для формата PCM.
	// Это оставшийся размер подцепочки, начиная с этой позиции.
	unsigned long subchunk1Size;

	// Аудио формат, полный список можно получить здесь http://audiocoding.ru/wav_formats.txt
	// Для PCM = 1 (то есть, Линейное квантование).
	// Значения, отличающиеся от 1, обозначают некоторый формат сжатия.
	unsigned short audioFormat;

	// Количество каналов. Моно = 1, Стерео = 2 и т.д.
	unsigned short numChannels;

	// Частота дискретизации. 8000 Гц, 44100 Гц и т.д.
	unsigned long sampleRate;

	// sampleRate * numChannels * bitsPerSample/8
	unsigned long byteRate;

	// numChannels * bitsPerSample/8
	// Количество байт для одного сэмпла, включая все каналы.
	unsigned short blockAlign;

	// Так называемая "глубиная" или точность звучания. 8 бит, 16 бит и т.д.
	unsigned short bitsPerSample;

	// Подцепочка "data" содержит аудио-данные и их размер.

	// Содержит символы "data"
	// (0x64617461 в big-endian представлении)
	char subchunk2Id[4];

	// numSamples * numChannels * bitsPerSample/8
	// Количество байт в области данных.
	unsigned long subchunk2Size;

	// Далее следуют непосредственно Wav данные.
};

static i2s_config_t i2s_config = {
	.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
	.sample_rate = 44100,
	.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, /* the DAC module will only take the 8bits from MSB */
	.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
	.communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_I2S_MSB,
	.intr_alloc_flags = 0, // default interrupt priority
	.dma_buf_count = 8,
	.dma_buf_len = 64,
	.use_apll = 0
};

void CmdWavheaderHandler(CIOBase &io, int argc, char *argv[])
{
	CFile file(argv[1], FM_READ);
	if (!file.IsOpened())
	{
		io << "Error opening file" << endl;
		return;
	}

	WAVHEADER header;
 
	file.GetBytes((char *)&header, sizeof(WAVHEADER));

	io << "Sample rate: " << (uint32_t)header.sampleRate << endl;
	io << "Channel: " << header.numChannels << endl;
	io << "Bits per sample: " << header.bitsPerSample << endl;

	i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);   //install and start i2s driver
	i2s_zero_dma_buffer(I2S_NUM_0);
    i2s_set_pin(I2S_NUM_0, NULL); //for internal DAC, this will enable both of the internal channels
	// i2s_set_sample_rates(I2S_NUM_0, header.sampleRate);
	i2s_set_clk(I2S_NUM_0, header.sampleRate, (i2s_bits_per_sample_t)header.bitsPerSample, (i2s_channel_t)header.numChannels);

    //You can call i2s_set_dac_mode to set built-in DAC output mode.
    i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);

	uint32_t len;
	size_t written;
	char buffer[64];
	while((len = file.GetBytes(buffer, 64)) > 0){
		i2s_write(I2S_NUM_0, buffer, len, &written, 100);
	}

	file.Close();
	i2s_driver_uninstall(I2S_NUM_0); //stop & destroy i2s driver

}
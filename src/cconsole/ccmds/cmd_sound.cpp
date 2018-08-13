#include <iobase/iobase.h>
#include <fileio.h>
#include <uart.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/i2s.h"

#define CCCC(c1, c2, c3, c4)    ((c4 << 24) | (c3 << 16) | (c2 << 8) | c1)

/* these are data structures to process wav file */
typedef enum headerState_e
{
	HEADER_RIFF,
	HEADER_FMT,
	HEADER_DATA,
	DATA
} headerState_t;

typedef struct wavRiff_s
{
	uint32_t chunkID;
	uint32_t chunkSize;
	uint32_t format;
} wavRiff_t;

typedef struct wavProperties_s
{
	uint32_t chunkID;
	uint32_t chunkSize;
	uint16_t audioFormat;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
} wavProperties_t;
/* variables hold file, state of process wav file and wav file properties */
headerState_t state = HEADER_RIFF;
wavProperties_t wavProps;

//i2s configuration
int i2s_num = 0; // i2s port number
static i2s_config_t i2s_config = {
	.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
	.sample_rate = 48000,
	.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, /* the DAC module will only take the 8bits from MSB */
	.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
	.communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_I2S_MSB,
	.intr_alloc_flags = 0, // default interrupt priority
	.dma_buf_count = 4,
	.dma_buf_len = 512,
	.use_apll = 0
};
//
/* write sample data to I2S */
int i2s_write_sample_nb(uint8_t sample)
{
	return i2s_write_bytes((i2s_port_t)i2s_num, (const char *)&sample, sizeof(uint8_t), 100);
}
/* read 4 bytes of data from wav file */
int read4bytes(CFile file, uint32_t *chunkId)
{
	// int n = file.read((uint8_t *)chunkId, sizeof(uint32_t));
	return file.GetString((char*)chunkId, sizeof(uint32_t));
}

int readbyte(CFile file, uint8_t *chunkId)
{
	// int n = file.read((uint8_t *)chunkId, sizeof(uint8_t));
	return file.GetString((char*)chunkId, sizeof(char));
}

/* these are function to process wav file */
int readRiff(CFile file, wavRiff_t *wavRiff)
{
	// int n = file.read((uint8_t *)wavRiff, sizeof(wavRiff_t));
	return file.GetString((char*)wavRiff, sizeof(wavRiff_t));
}
int readProps(CFile file, wavProperties_t *wavProps)
{
	// int n = file.read((uint8_t *)wavProps, sizeof(wavProperties_t));
	return file.GetString((char*)wavProps, sizeof(wavProperties_t));
}

void CmdSoundHandler(CIOBase &io, int argc, char *argv[]){
	const char *filename = argv[1];

	CFile root(filename, FM_READ);
	if (root.IsOpened())
	{
		int c = 0;
		int n;
		while (root.GetBufferedDataLength())
		{
			switch (state)
			{
			case HEADER_RIFF:
				wavRiff_t wavRiff;
				n = readRiff(root, &wavRiff);
				if (n == sizeof(wavRiff_t))
				{
					if (wavRiff.chunkID == CCCC('R', 'I', 'F', 'F') && wavRiff.format == CCCC('W', 'A', 'V', 'E'))
					{
						state = HEADER_FMT;
						io << "HEADER_RIFF" << endl;
					}
				}
				break;
			case HEADER_FMT:
				n = readProps(root, &wavProps);
				io << "HEADER_FMT: received: " << n << " expected: " << (int)sizeof(wavProperties_t) << endl;
				if (n == sizeof(wavProperties_t))
				{
					state = HEADER_DATA;
				}
				break;
			case HEADER_DATA:
				uint32_t chunkId, chunkSize;
				n = read4bytes(root, &chunkId);
				if (n == 4)
				{
					if (chunkId == CCCC('d', 'a', 't', 'a'))
					{
						io << "HEADER_DATA" << endl;
					}
				}
				n = read4bytes(root, &chunkSize);
				if (n == 4)
				{
					io << "Prepare data" << endl;
					state = DATA;
				}
				//initialize i2s with configurations above
				i2s_driver_install((i2s_port_t)i2s_num, &i2s_config, 0, NULL);
				i2s_set_pin((i2s_port_t)i2s_num, NULL);
				//set sample rates of i2s to sample rate of wav file
				i2s_set_sample_rates((i2s_port_t)i2s_num, wavProps.sampleRate);
				break;
			/* after processing wav file, it is time to process music data */
			case DATA:
				uint8_t data;
				n = readbyte(root, &data);
				i2s_write_sample_nb(data);
				break;
			}
		}
		root.Close();
	}
	else
	{
		io << "Error opening file" << endl;
	}
	i2s_driver_uninstall((i2s_port_t)i2s_num); //stop & destroy i2s driver
	io << "Done" << endl;
}
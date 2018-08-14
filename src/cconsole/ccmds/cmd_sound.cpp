#include <iobase/iobase.h>
#include <fileio.h>
#include <uart.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/i2s.h"
#include "string.h"

#include "drivers/mad/mad.h"
#include "drivers/mad/stream.h"
#include "drivers/mad/synth.h"
#include "drivers/mad/frame.h"

static i2s_config_t i2s_config = {
	.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_PDM),
	.sample_rate = 44100,
	.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, /* the DAC module will only take the 8bits from MSB */
	.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
	.communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_I2S_MSB,
	.intr_alloc_flags = 0, // default interrupt priority
	.dma_buf_count = 32,
	.dma_buf_len = 64,
	.use_apll = 0
};

// The theoretical maximum frame size is 2881 bytes,
// MPEG 2.5 Layer II, 8000 Hz @ 160 kbps, with a padding slot plus 8 byte MAD_BUFFER_GUARD.
#define MAX_FRAME_SIZE (2889)

// The theoretical minimum frame size of 24 plus 8 byte MAD_BUFFER_GUARD.
#define MIN_FRAME_SIZE (32)

#define PAYLOAD_SIZE 4096

typedef struct mad_stream mad_stream_t;
typedef struct mad_frame mad_frame_t;
typedef struct mad_synth mad_synth_t;

static uint64_t buf_underrun_cnt;

/* Check out https://stackoverflow.com/questions/39803572/libmad-playback-too-fast-if-read-in-chunks for better understand MAD input */
static enum mad_flow input(CFile *file, struct mad_stream *stream, uint8_t *buff, uint32_t buffSize)
{
	uint32_t bytes_consumed = stream->next_frame - stream->buffer;
	uint32_t diff = file->GetSize() - bytes_consumed;

	if(diff == 0)
		return MAD_FLOW_STOP;

	uint32_t len = 0, keep = 0;
	uint8_t eof = 0;

	if(stream->error != MAD_ERROR_BUFLEN){
		keep = 0;
	} else if(stream->next_frame != NULL) {
		keep = stream->bufend - stream->next_frame;
	} else if((stream->bufend - stream->buffer) < PAYLOAD_SIZE) {
		keep = PAYLOAD_SIZE - MAX_FRAME_SIZE;
	}

	if(keep)
		memmove(buff, stream->bufend - keep, keep);

	len = PAYLOAD_SIZE - keep;
	int result = file->GetBytes((char*)(buff + keep), len);

	if(result < 0)
		return MAD_FLOW_STOP;
	else if(result == 0){
		if(keep + MAD_BUFFER_GUARD < PAYLOAD_SIZE){
			memset(buff + keep, 0, MAD_BUFFER_GUARD);
			len = keep + MAD_BUFFER_GUARD;
			eof = 1;
		} else {
			memset(buff + keep, 0, PAYLOAD_SIZE - keep);
			len = PAYLOAD_SIZE;
		}
	} else {
		len = keep + result;
	}

	mad_stream_buffer(stream, buff, len);
	return eof ? MAD_FLOW_STOP : MAD_FLOW_CONTINUE;
}

static enum mad_flow error(void *data, struct mad_stream *stream, struct mad_frame *frame)
{
	uart << "Decode error: " << stream->error << " : " << mad_stream_errorstr(stream) << endl;
	return MAD_FLOW_CONTINUE;
}

extern "C" void set_dac_sample_rate(int rate)
{
	i2s_set_sample_rates(I2S_NUM_0, rate);
}

/* render callback for the libmad synth */
extern "C" void render_sample_block(short *sample_buff_ch0, short *sample_buff_ch1, int num_samples, unsigned int num_channels)
{
	uint32_t *tmp = (uint32_t*)malloc(num_samples * sizeof(uint32_t));
	assert(tmp);

	uint16_t *samples = (uint16_t*)sample_buff_ch0;
	for(int i = 0; i < num_samples; i++){
		// samples[i] += 0x8000;
		// samples[num_samples + i] += 0x8000;

		tmp[i] = sample_buff_ch0[i] << 16;
		tmp[i] |= sample_buff_ch0[num_samples + i];
	}

	uint32_t len = num_samples * sizeof(uint32_t);

	size_t written, total = 0;
	do{
		// i2s_write(I2S_NUM_0, ((char*)tmp) + total, len - total, &written, 100);
		i2s_write(I2S_NUM_0, samples + total, num_samples * 2 - total, &written, 100);
		total += written;
	} while(total < num_samples * 2);

	free(tmp);
	return;
}

void CmdSoundHandler(CIOBase &io, int argc, char *argv[])
{
	const char *filename = argv[1];

	CFile root(filename, FM_READ);
	if (!root.IsOpened())
	{
		io << "Cannot open file: " << filename << endl;
		return;
	}

	int ret;
	struct mad_stream *stream;
	struct mad_frame *frame;
	struct mad_synth *synth;

	//Allocate structs needed for mp3 decoding
	stream = (mad_stream_t *)malloc(sizeof(struct mad_stream));
	frame = (mad_frame_t *)malloc(sizeof(struct mad_frame));
	synth = (mad_synth_t *)malloc(sizeof(struct mad_synth));
	uint8_t *buf = (uint8_t *)malloc(PAYLOAD_SIZE);

	assert(stream && frame && synth && buf);

	buf_underrun_cnt = 0;

	io << "Starting..." << endl;

	mad_stream_init(stream);
	mad_frame_init(frame);
	mad_synth_init(synth);

	i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);   //install and start i2s driver
	i2s_set_pin(I2S_NUM_0, NULL); //for internal DAC, this will enable both of the internal channels
	i2s_zero_dma_buffer(I2S_NUM_0);

	while (1)
	{
		// calls mad_stream_buffer internally
		if (input(&root, stream, buf, PAYLOAD_SIZE) == MAD_FLOW_STOP)
		{
			break;
		}

		// decode frames until MAD complains
		while (1)
		{

			// if(player->decoder_command == CMD_STOP) {
			//     goto abort;
			// }

			// returns 0 or -1
			ret = mad_frame_decode(frame, stream);

			if (ret == -1)
			{
				if (!MAD_RECOVERABLE(stream->error))
				{
					//We're most likely out of buffer and need to call input() again
					break;
				}
				error(NULL, stream, frame);
				continue;
			}

			mad_synth_frame(synth, frame);
		}
		// ESP_LOGI(TAG, "RAM left %d", esp_get_free_heap_size());
	}

	i2s_driver_uninstall(I2S_NUM_0); //stop & destroy i2s driver

	free(stream);
	free(frame);
	free(synth);
	free(buf);

	root.Close();
	io << "Done" << endl;
}
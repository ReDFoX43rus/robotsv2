#include <iobase/iobase.h>
#include <fileio.h>
#include <uart.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/i2s.h"

#include "drivers/mad/mad.h"
#include "drivers/mad/stream.h"
#include "drivers/mad/synth.h"
#include "drivers/mad/frame.h"

// The theoretical maximum frame size is 2881 bytes,
// MPEG 2.5 Layer II, 8000 Hz @ 160 kbps, with a padding slot plus 8 byte MAD_BUFFER_GUARD.
#define MAX_FRAME_SIZE (2889)

// The theoretical minimum frame size of 24 plus 8 byte MAD_BUFFER_GUARD.
#define MIN_FRAME_SIZE (32)

typedef struct mad_stream mad_stream_t;
typedef struct mad_frame mad_frame_t;
typedef struct mad_synth mad_synth_t;

static uint64_t buf_underrun_cnt;

static enum mad_flow input(CFile *file, struct mad_stream *stream, uint8_t *buff, uint32_t buffSize)
{
	// next_frame is the position MAD is interested in resuming from
	uint32_t bytes_consumed = stream->next_frame - stream->buffer;

	uart << "Bytes consumed: " << bytes_consumed << endl;

	uint32_t diff = file->GetSize() - bytes_consumed;
	if(diff == 0)
		return MAD_FLOW_STOP;

	uint32_t len = diff < buffSize ? diff : buffSize;
	file->GetString((char*)buff, len);

	// Okay, let MAD decode the buffer.
	mad_stream_buffer(stream, buff, len);
	return MAD_FLOW_CONTINUE;
}

//Routine to print out an error
static enum mad_flow error(void *data, struct mad_stream *stream, struct mad_frame *frame)
{
	// printf("dec err 0x%04x (%s)\n", stream->error, mad_stream_errorstr(stream));
	uart << "Decode error: " << stream->error << " : " << mad_stream_errorstr(stream) << endl;
	return MAD_FLOW_CONTINUE;
}

extern "C" void set_dac_sample_rate(int rate)
{
}

/* render callback for the libmad synth */
extern "C" void render_sample_block(short *sample_buff_ch0, short *sample_buff_ch1, int num_samples, unsigned int num_channels)
{
	uint32_t len = num_samples * sizeof(short) * num_channels;
	// render_samples((char *)sample_buff_ch0, len, &mad_buffer_fmt);

	return;
}

void CmdSoundHandler(CIOBase &io, int argc, char *argv[])
{
	const char *filename = argv[1];

	FILE *f = fopen(filename, "r");
	if(!f){
		io << "Cannot open file via C" << endl;
		return;
	}
	fclose(f);

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
	uint8_t *buf = (uint8_t *)malloc(MAX_FRAME_SIZE);

	assert(stream && frame && synth && buf);

	buf_underrun_cnt = 0;

	io << "Starting..." << endl;

	mad_stream_init(stream);
	mad_frame_init(frame);
	mad_synth_init(synth);

	while (1)
	{
		// calls mad_stream_buffer internally
		if (input(&root, stream, buf, MAX_FRAME_SIZE) == MAD_FLOW_STOP)
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

	free(stream);
	free(frame);
	free(synth);
	free(buf);

	root.Close();
	io << "Done" << endl;
}
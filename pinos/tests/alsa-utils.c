#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <errno.h>
#include <getopt.h>
#include <sys/time.h>
#include <math.h>

static int verbose = 0;					/* verbose flag */

#if 0
static void
generate_sine(const snd_pcm_channel_area_t *areas,
			  snd_pcm_uframes_t offset,
			  int count, double *_phase)
{
	static double max_phase = 2. * M_PI;
	double phase = *_phase;
	double step = max_phase*freq/(double)rate;
	unsigned char *samples[channels];
	int steps[channels];
	unsigned int chn;
	int format_bits = snd_pcm_format_width(format);
	unsigned int maxval = (1 << (format_bits - 1)) - 1;
	int bps = format_bits / 8;  /* bytes per sample */
	int phys_bps = snd_pcm_format_physical_width(format) / 8;
	int big_endian = snd_pcm_format_big_endian(format) == 1;
	int to_unsigned = snd_pcm_format_unsigned(format) == 1;
	int is_float = (format == SND_PCM_FORMAT_FLOAT_LE ||
			format == SND_PCM_FORMAT_FLOAT_BE);

	/* verify and prepare the contents of areas */
	for (chn = 0; chn < channels; chn++) {
		if ((areas[chn].first % 8) != 0) {
			printf("areas[%i].first == %i, aborting...\n", chn, areas[chn].first);
			exit(EXIT_FAILURE);
		}
		samples[chn] = /*(signed short *)*/(((unsigned char *)areas[chn].addr) + (areas[chn].first / 8));
		if ((areas[chn].step % 16) != 0) {
			printf("areas[%i].step == %i, aborting...\n", chn, areas[chn].step);
			exit(EXIT_FAILURE);
		}
		steps[chn] = areas[chn].step / 8;
		samples[chn] += offset * steps[chn];
	}
	/* fill the channel areas */
	while (count-- > 0) {
		union {
			float f;
			int i;
		} fval;
		int res, i;
		if (is_float) {
			fval.f = sin(phase) * maxval;
			res = fval.i;
		} else
			res = sin(phase) * maxval;
		if (to_unsigned)
			res ^= 1U << (format_bits - 1);
		for (chn = 0; chn < channels; chn++) {
			/* Generate data in native endian format */
			if (big_endian) {
				for (i = 0; i < bps; i++)
					*(samples[chn] + phys_bps - 1 - i) = (res >> i * 8) & 0xff;
			} else {
				for (i = 0; i < bps; i++)
					*(samples[chn] + i) = (res >>  i * 8) & 0xff;
			}
			samples[chn] += steps[chn];
		}
		phase += step;
		if (phase >= max_phase)
			phase -= max_phase;
	}
	*_phase = phase;
}
#endif

#define CHECK(s,msg) if ((err = (s)) < 0) { printf (msg ": %s\n", snd_strerror(err)); return err; }

static snd_pcm_format_t
spi_alsa_format_to_alsa (const char *format)
{
  if (strcmp (format, "S8") == 0)
    return SND_PCM_FORMAT_S8;
  if (strcmp (format, "U8") == 0)
    return SND_PCM_FORMAT_U8;
    /* 16 bit */
  if (strcmp (format, "S16LE") == 0)
    return SND_PCM_FORMAT_S16_LE;
  if (strcmp (format, "S16BE") == 0)
    return SND_PCM_FORMAT_S16_BE;
  if (strcmp (format, "U16LE") == 0)
    return SND_PCM_FORMAT_U16_LE;
  if (strcmp (format, "U16BE") == 0)
    return SND_PCM_FORMAT_U16_BE;
    /* 24 bit in low 3 bytes of 32 bits */
  if (strcmp (format, "S24_32LE") == 0)
    return SND_PCM_FORMAT_S24_LE;
  if (strcmp (format, "S24_32BE") == 0)
    return SND_PCM_FORMAT_S24_BE;
  if (strcmp (format, "U24_32LE") == 0)
    return SND_PCM_FORMAT_U24_LE;
  if (strcmp (format, "U24_32BE") == 0)
    return SND_PCM_FORMAT_U24_BE;
    /* 24 bit in 3 bytes */
  if (strcmp (format, "S24LE") == 0)
    return SND_PCM_FORMAT_S24_3LE;
  if (strcmp (format, "S24BE") == 0)
    return SND_PCM_FORMAT_S24_3BE;
  if (strcmp (format, "U24LE") == 0)
    return SND_PCM_FORMAT_U24_3LE;
  if (strcmp (format, "U24BE") == 0)
    return SND_PCM_FORMAT_U24_3BE;
    /* 32 bit */
  if (strcmp (format, "S32LE") == 0)
    return SND_PCM_FORMAT_S32_LE;
  if (strcmp (format, "S32BE") == 0)
    return SND_PCM_FORMAT_S32_BE;
  if (strcmp (format, "U32LE") == 0)
    return SND_PCM_FORMAT_U32_LE;
  if (strcmp (format, "U32BE") == 0)
    return SND_PCM_FORMAT_U32_BE;

  return SND_PCM_FORMAT_UNKNOWN;
}

static int
set_hwparams (SpiALSASink *this)
{
  unsigned int rrate;
  snd_pcm_uframes_t size;
  int err, dir;
  snd_pcm_hw_params_t *params;
  snd_pcm_format_t format;
  SpiALSAState *state = &this->state;
  SpiALSASinkFormat *fmt = &this->current_format;
  snd_pcm_t *handle = state->handle;
  unsigned int buffer_time;
  unsigned int period_time;

  snd_pcm_hw_params_alloca (&params);
  /* choose all parameters */
  CHECK (snd_pcm_hw_params_any (handle, params), "Broken configuration for playback: no configurations available");
  /* set hardware resampling */
  CHECK (snd_pcm_hw_params_set_rate_resample (handle, params, 0), "set_rate_resample");
  /* set the interleaved read/write format */
  CHECK (snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_MMAP_INTERLEAVED), "set_access");

  /* set the sample format */
  format = spi_alsa_format_to_alsa (fmt->format);
  printf ("Stream parameters are %iHz, %s, %i channels\n", fmt->samplerate, snd_pcm_format_name(format), fmt->channels);
  CHECK (snd_pcm_hw_params_set_format (handle, params, format), "set_format");
  /* set the count of channels */
  CHECK (snd_pcm_hw_params_set_channels (handle, params, fmt->channels), "set_channels");
  /* set the stream rate */
  rrate = fmt->samplerate;
  CHECK (snd_pcm_hw_params_set_rate_near (handle, params, &rrate, 0), "set_rate_near");
  if (rrate != fmt->samplerate) {
    printf("Rate doesn't match (requested %iHz, get %iHz)\n", fmt->samplerate, rrate);
    return -EINVAL;
  }
  /* set the buffer time */
  buffer_time = this->params.buffer_time;
  CHECK (snd_pcm_hw_params_set_buffer_time_near (handle, params, &buffer_time, &dir), "set_buffer_time_near");
  CHECK (snd_pcm_hw_params_get_buffer_size (params, &size), "get_buffer_size");
  state->buffer_size = size;

  /* set the period time */
  period_time = this->params.period_time;
  CHECK (snd_pcm_hw_params_set_period_time_near (handle, params, &period_time, &dir), "set_period_time_near");
  CHECK (snd_pcm_hw_params_get_period_size (params, &size, &dir), "get_period_size");
  state->period_size = size;

  /* write the parameters to device */
  CHECK (snd_pcm_hw_params (handle, params), "set_hw_params");

  return 0;
}

static int
set_swparams (SpiALSASink *this)
{
  SpiALSAState *state = &this->state;
  snd_pcm_t *handle = state->handle;
  int err = 0;
  snd_pcm_sw_params_t *params;

  snd_pcm_sw_params_alloca (&params);

  /* get the current params */
  CHECK (snd_pcm_sw_params_current (handle, params), "sw_params_current");
  /* start the transfer when the buffer is almost full: */
  /* (buffer_size / avail_min) * avail_min */
  CHECK (snd_pcm_sw_params_set_start_threshold (handle, params,
        (state->buffer_size / state->period_size) * state->period_size), "set_start_threshold");

  /* allow the transfer when at least period_size samples can be processed */
  /* or disable this mechanism when period event is enabled (aka interrupt like style processing) */
  CHECK (snd_pcm_sw_params_set_avail_min (handle, params,
        this->params.period_event ? state->buffer_size : state->period_size), "set_avail_min");
  /* enable period events when requested */
  if (this->params.period_event) {
    CHECK (snd_pcm_sw_params_set_period_event (handle, params, 1), "set_period_event");
  }
  /* write the parameters to the playback device */
  CHECK (snd_pcm_sw_params (handle, params), "sw_params");

  return 0;
}

/*
 *   Underrun and suspend recovery
 */
static int
xrun_recovery (snd_pcm_t *handle, int err)
{
  if (verbose)
    printf("stream recovery\n");
  if (err == -EPIPE) {	/* under-run */
    err = snd_pcm_prepare(handle);
    if (err < 0)
      printf("Can't recovery from underrun, prepare failed: %s\n", snd_strerror(err));
    return 0;
  } else if (err == -ESTRPIPE) {
    while ((err = snd_pcm_resume(handle)) == -EAGAIN)
      sleep(1);	/* wait until the suspend flag is released */
    if (err < 0) {
      err = snd_pcm_prepare(handle);
      if (err < 0)
        printf("Can't recovery from suspend, prepare failed: %s\n", snd_strerror(err));
    }
    return 0;
  }
  return err;
}

#if 0
/*
 *   Transfer method - write and wait for room in buffer using poll
 */
static int
wait_for_poll (snd_pcm_t *handle, struct pollfd *ufds, unsigned int count)
{
  unsigned short revents;

  while (1) {
    poll(ufds, count, -1);
    snd_pcm_poll_descriptors_revents(handle, ufds, count, &revents);
    if (revents & POLLERR)
      return -EIO;
    if (revents & POLLOUT)
      return 0;
  }
}
#endif

/*
 *   Transfer method - direct write only
 */
static void *
direct_loop (void *user_data)
{
  SpiALSASink *this = user_data;
  SpiALSAState *state = &this->state;
  snd_pcm_t *handle = state->handle;
  const snd_pcm_channel_area_t *my_areas;
  snd_pcm_uframes_t offset, frames, size;
  snd_pcm_sframes_t avail, commitres;
  snd_pcm_state_t st;
  int err, first = 1;

  while (state->running) {
    st = snd_pcm_state(handle);
    if (st == SND_PCM_STATE_XRUN) {
      err = xrun_recovery(handle, -EPIPE);
      if (err < 0) {
        printf("XRUN recovery failed: %s\n", snd_strerror(err));
        return NULL;
      }
      first = 1;
    } else if (st == SND_PCM_STATE_SUSPENDED) {
      err = xrun_recovery(handle, -ESTRPIPE);
      if (err < 0) {
        printf("SUSPEND recovery failed: %s\n", snd_strerror(err));
        return NULL;
      }
    }
    avail = snd_pcm_avail_update(handle);
    if (avail < 0) {
      err = xrun_recovery(handle, avail);
      if (err < 0) {
        printf("avail update failed: %s\n", snd_strerror(err));
        return NULL;
      }
      first = 1;
      continue;
    }
    if (avail < state->period_size) {
      if (first) {
        first = 0;
        err = snd_pcm_start(handle);
        if (err < 0) {
          printf("Start error: %s\n", snd_strerror(err));
          exit(EXIT_FAILURE);
        }
      } else {
        err = snd_pcm_wait(handle, -1);
        if (err < 0) {
          if ((err = xrun_recovery(handle, err)) < 0) {
            printf("snd_pcm_wait error: %s\n", snd_strerror(err));
            exit(EXIT_FAILURE);
          }
          first = 1;
        }
      }
      continue;
    }
    size = state->period_size;
    while (size > 0) {
      frames = size;
      err = snd_pcm_mmap_begin(handle, &my_areas, &offset, &frames);
      if (err < 0) {
        if ((err = xrun_recovery(handle, err)) < 0) {
          printf("MMAP begin avail error: %s\n", snd_strerror(err));
          exit(EXIT_FAILURE);
        }
        first = 1;
      }

      {
        SpiEvent event;
        ALSABuffer *buffer = &this->buffer;

        event.refcount = 1;
        event.notify = NULL;
        event.type = SPI_EVENT_TYPE_REQUEST_DATA;
        event.port_id = 0;
        event.data = buffer;

        buffer->buffer.refcount = 1;
        buffer->buffer.notify = NULL;
        buffer->buffer.size = sizeof (ALSABuffer);
        buffer->buffer.n_metas = 1;
        buffer->buffer.metas = buffer->meta;
        buffer->buffer.n_datas = 1;
        buffer->buffer.datas = buffer->data;

        buffer->header.flags = 0;
        buffer->header.seq = 0;
        buffer->header.pts = 0;
        buffer->header.dts_offset = 0;

        buffer->meta[0].type = SPI_META_TYPE_HEADER;
        buffer->meta[0].data = &buffer->header;
        buffer->meta[0].size = sizeof (buffer->header);

        buffer->data[0].type = SPI_DATA_TYPE_MEMPTR;
        buffer->data[0].data = (uint8_t *)my_areas[0].addr + (offset * sizeof (uint16_t) * 2);
        buffer->data[0].size = frames * sizeof (uint16_t) * 2;

        this->event_cb (&this->node, &event,this->user_data);

        spi_buffer_unref ((SpiBuffer *)event.data);
      }
      if (this->input_buffer) {
        if (this->input_buffer != &this->buffer.buffer) {
          /* FIXME, copy input */
        }
        spi_buffer_unref (this->input_buffer);
        this->input_buffer = NULL;
      }

      commitres = snd_pcm_mmap_commit(handle, offset, frames);
      if (commitres < 0 || (snd_pcm_uframes_t)commitres != frames) {
        if ((err = xrun_recovery(handle, commitres >= 0 ? -EPIPE : commitres)) < 0) {
          printf("MMAP commit error: %s\n", snd_strerror(err));
          exit(EXIT_FAILURE);
        }
        first = 1;
      }
      size -= frames;
    }
  }
  return NULL;
}

static int
spi_alsa_open (SpiALSASink *this)
{
  SpiALSAState *state = &this->state;
  int err;

  CHECK (snd_output_stdio_attach (&state->output, stdout, 0), "attach failed");

  printf ("Playback device is '%s'\n", this->params.device);
  CHECK (snd_pcm_open (&state->handle,
                       this->params.device,
                       SND_PCM_STREAM_PLAYBACK,
                       SND_PCM_NONBLOCK |
                       SND_PCM_NO_AUTO_RESAMPLE |
                       SND_PCM_NO_AUTO_CHANNELS |
                       SND_PCM_NO_AUTO_FORMAT), "open failed");

  return 0;
}

static int
spi_alsa_start (SpiALSASink *this)
{
  SpiALSAState *state = &this->state;
  int err;

  CHECK (set_hwparams (this), "hwparams");
  CHECK (set_swparams (this), "swparams");

  snd_pcm_dump (this->state.handle, this->state.output);

  state->running = true;
  if ((err = pthread_create (&state->thread, NULL, direct_loop, this)) != 0) {
    printf ("can't create thread: %d", err);
    state->running = false;
  }
  return err;
}

static int
spi_alsa_stop (SpiALSASink *this)
{
  SpiALSAState *state = &this->state;

  if (state->running) {
    state->running = false;
    pthread_join (state->thread, NULL);
  }
  return 0;
}

static int
spi_alsa_close (SpiALSASink *this)
{
  SpiALSAState *state = &this->state;
  int err = 0;

  CHECK (snd_pcm_close (state->handle), "close failed");

  return err;
}

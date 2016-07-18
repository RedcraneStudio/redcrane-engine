/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "flac.h"

#include <vector>

#include "FLAC/stream_decoder.h"

#include "../common/log.h"
namespace redc { namespace snd
{
  FLAC__StreamDecoderWriteStatus
  flac_write_fn(FLAC__StreamDecoder const*,
                FLAC__Frame const* frame,
                FLAC__int32 const* const buffer[],
                void* client_data) noexcept
  {
    auto pcm_data = reinterpret_cast<PCM_Data*>(client_data);

    for(unsigned int i = 0; i < frame->header.blocksize; ++i)
    {
      if(pcm_data->bits_per_sample == 16)
      {
        // Clip
        int64_t left = buffer[0][i];
        if(left > 32767) left = 32767;
        if(-32767 > left) left = -32767;

        int64_t right = buffer[1][i];
        if(right > 32767) right = 32767;
        if(-32767 > right) right = -32767;

        pcm_data->samples.push_back(Sample{
          static_cast<int16_t>(left),
          static_cast<int16_t>(right)});
      }
    }

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
  }

  void flac_metadata_fn(FLAC__StreamDecoder const* decoder,
                        FLAC__StreamMetadata const* metadata,
                        void* client_data) noexcept
  {
    if(metadata->type == FLAC__METADATA_TYPE_STREAMINFO)
    {
      log_i("Total samples: %", metadata->data.stream_info.total_samples);
      log_i("Sample rate: %", metadata->data.stream_info.sample_rate);
      log_i("Channels: %", metadata->data.stream_info.channels);
      log_i("Bits per sample: %", metadata->data.stream_info.bits_per_sample);

      auto* pcm_data = reinterpret_cast<PCM_Data*>(client_data);
      pcm_data->bits_per_sample = metadata->data.stream_info.bits_per_sample;
      if(pcm_data->bits_per_sample != 16)
      {
        log_e("We only support signed 16bit little-endian PCM data.");
      }
    }
  }

  void flac_error_fn(FLAC__StreamDecoder const* decoder,
                     FLAC__StreamDecoderErrorStatus status,
                     void* client_data) noexcept
  {
    log_e("Error encountered parsing FLAC file.");
    reinterpret_cast<PCM_Data*>(client_data)->error = true;
  }

  PCM_Data load_flac(std::string filename) noexcept
  {
    auto stream_decoder = FLAC__stream_decoder_new();
    if(!stream_decoder)
    {
      log_e("Failed to initialize libFLAC stream decoder.");
    }

    PCM_Data pcm_data;
    pcm_data.error = false;

    FLAC__stream_decoder_init_file(stream_decoder, filename.c_str(),
                                   flac_write_fn, flac_metadata_fn,
                                   flac_error_fn, &pcm_data);

    FLAC__stream_decoder_process_until_end_of_stream(stream_decoder);

    FLAC__stream_decoder_finish(stream_decoder);
    FLAC__stream_decoder_delete(stream_decoder);

    return pcm_data;
  }
} }

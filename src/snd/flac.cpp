/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "flac.h"

#include <vector>

#include "FLAC/stream_decoder.h"

#include "../common/log.h"
namespace game { namespace snd
{
  FLAC__StreamDecoderWriteStatus
  flac_write_fn(FLAC__StreamDecoder const*,
                FLAC__Frame const* frame,
                FLAC__int32 const* const buffer[],
                void* client_data) noexcept
  {
    auto pcm_data_out = reinterpret_cast<PCM_Data*>(client_data);

    for(unsigned int i = 0; i < frame->header.blocksize; ++i)
    {
      auto sample = Sample{};

      sample.left = buffer[0][i];
      sample.right = buffer[1][i];

      pcm_data_out->samples.push_back(sample);
    }

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
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

    FLAC__stream_decoder_init_file(stream_decoder, filename.c_str(),
                                   flac_write_fn, NULL, flac_error_fn,
                                   &pcm_data);

    FLAC__stream_decoder_process_until_end_of_stream(stream_decoder);

    FLAC__stream_decoder_finish(stream_decoder);
    FLAC__stream_decoder_delete(stream_decoder);

    return pcm_data;
  }
} }

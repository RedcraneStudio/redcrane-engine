/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#include <chrono>
#include "assets/minigltf.h"
#include "common/log.h"
int main(int argc, char** argv)
{
  redc::Scoped_Log_Init log_raii{};
  if(argc < 2)
  {
    redc::log_e("usage: % <filename.gltf>", argv[0]);
    return EXIT_FAILURE;
  }

  auto before = std::chrono::high_resolution_clock::now();
  auto desc = redc::load_gltf_file(argv[1]);
  auto after = std::chrono::high_resolution_clock::now();

  auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
  redc::log_i("Took %s to load %", dt.count() / 1000.0,
              desc.filepath.value().native());

  for(auto const& buf : desc.buffers)
  {
    redc::log_i("buffer.name: %", buf.second.name);
    redc::log_i("\tbuffer.buf: %", (intptr_t) &buf.second.buf[0]);
    redc::flush_log_full();
  }

  for(auto const& buf_view : desc.buf_views)
  {
    redc::log_i("bufferView.name: %", buf_view.second.name);
    redc::log_i("\tbufferView.base: %", (intptr_t) buf_view.second.base_ptr);
    redc::log_i("\tbufferView.size: %", buf_view.second.size);
    redc::log_i("\tbufferView.target: %",
                static_cast<unsigned int>(buf_view.second.target));
    redc::flush_log_full();
  }
  for(auto const& accessor : desc.accessors)
  {
    redc::log_i("accessor.name: %", accessor.second.name);
    redc::log_i("\taccessor.base_ptr: %", (intptr_t) accessor.second.base_ptr);
    redc::log_i("\taccessor.count: %", accessor.second.count);
    redc::log_i("\taccessor.stride: %", accessor.second.stride);
    redc::log_i("\taccessor.component_type: %",
                (unsigned int) accessor.second.component_type);
    redc::log_i("\taccessor.attribute_type: %",
                (unsigned int) accessor.second.attribute_type);
    redc::flush_log_full();
  }

  return EXIT_SUCCESS;
}

/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#include "terrain.h"

#include <vector>

#include "../common/log.h"

extern "C"
{
  #include "open-simplex-noise.h"
}
#include <png.h>

namespace game { namespace strat
{
  void gen_noise_heightmap(int64_t seed, Value_Map<float>& map) noexcept
  {
    osn_context* context;
    open_simplex_noise(seed, &context);

    for(int i = 0; i < map.extents.x * map.extents.y; ++i)
    {
      auto x = i % map.extents.x;
      auto y = i / map.extents.x;

      open_simplex_noise2(context, x, y);
    }

    open_simplex_noise_free(context);
  }

  void write_png_heightmap(Value_Map<float> const& map,
                           std::string const& filename) noexcept
  {
    // All this code is pretty much based completely on the libpng manual so
    // check that out.

    std::FILE* fp = std::fopen(filename.data(), "wb");
    if(!fp)
    {
      log_w("Failed to open file '%'", filename);
      return;
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                  NULL, NULL, NULL);
    if(!png_ptr)
    {
      log_w("Failed to initialize libpng to write file '%'", filename);
      return;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr)
    {
      png_destroy_write_struct(&png_ptr, NULL);
      log_w("Failed to initialize png info to write '%'", filename);
      return;
    }

    if(setjmp(png_jmpbuf(png_ptr)))
    {
      png_destroy_write_struct(&png_ptr, &info_ptr);
      fclose(fp);

      log_w("libpng error for '%'", filename);
      return;
    }

    png_init_io(png_ptr, fp);

    png_set_IHDR(png_ptr, info_ptr, map.extents.x, map.extents.y, 8,
                 PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    std::vector<uint8_t*> ptrs;
    for(int i = 0; i < map.extents.y; ++i)
    {
      ptrs.push_back(new uint8_t[map.extents.x * 3]);
      for(int j = 0; j < map.extents.x; ++j)
      {
        auto fl_val = (map.values[i * map.extents.x + j] + 1.0f) / 2.0f;
        uint8_t value = std::max(0.0f, std::min(fl_val * 0xff, (float) 0xff));
        ptrs.back()[j * 3] = value;
        ptrs.back()[j * 3 + 1] = value;
        ptrs.back()[j * 3 + 2] = value;
      }
    }

    png_set_rows(png_ptr, info_ptr, &ptrs[0]);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    for(auto* ptr : ptrs) delete ptr;

    png_destroy_write_struct(&png_ptr, &info_ptr);
  }
} }

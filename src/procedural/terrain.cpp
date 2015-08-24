/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#include "terrain.h"

#include <vector>

#include "../common/log.h"

#include <png.h>

#include "../common/debugging.h"

namespace game { namespace strat
{
  Noise_Raii::Noise_Raii(int64_t seed, osn_context** ptr) noexcept
  {
    // Pointer to an empty pointer given, allocate it and point our parameter
    // to it.
    if(!(*ptr))
    {
      open_simplex_noise(seed, ptr);
      ptr_ = *ptr;
      allocated_ = true;
    }
    else allocated_ = false;

    // If the ptr was valid it doesn't need to be initialized so don't do it.

  }
  Noise_Raii::~Noise_Raii() noexcept
  {
    if(allocated_) open_simplex_noise_free(ptr_);
  }

  // This function could be run alone.
  void terrain_v1_radial_land(Grid_Map& map, Terrain_Params const& tp) noexcept
  {
    osn_context* osn = nullptr;
    auto noise_init = Noise_Raii{tp.seed, &osn};

    for(int i = 0; i < map.extents.y; ++i)
    {
      for(int j = 0; j < map.extents.x; ++j)
      {
        // Point from the continent origin to here.
        Vec<float> to_here = Vec<int>{j, i} - tp.origin;
        auto to_here_dir = normalize(to_here);

        // Tp size is more like diameter, but not necessarily any particular
        // value.
        auto radius = tp.size / 2.0f;

        // Given the direction and radius find the circle edge. This value
        // should be the same for every vector with equal direction, but not
        // necessarily length.
        auto circle_edge = tp.origin + to_here_dir * radius;

        // Angle from origin
        auto angle = std::atan2(to_here_dir.y, to_here_dir.x);
        // Get a small delta from the angle. We will use this to modify the
        auto delta = open_simplex_noise2(osn, to_here_dir.y,
                                         to_here_dir.x) * tp.radial.amplitude;

        auto modified_radius = radius + delta;

        // This tile is land.
        if(length(to_here) < modified_radius)
        {
          map.values[i * map.extents.x + j].type = Cell_Type::Land;
        }
        else
        {
          map.values[i * map.extents.x + j].type = Cell_Type::Water;
        }
      }
    }
  }

  void terrain_v1_map(Grid_Map& map, Terrain_Params const& ter_params) noexcept
  {
    switch(ter_params.algorithm)
    {
    case Landmass_Algorithm::Radial:
      terrain_v1_radial_land(map, ter_params);
      break;
    case Landmass_Algorithm::Other:
      ter_params.other.gen_fn(map, ter_params);
    }
  }

  void write_png_heightmap(Grid_Map const& map,
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
        uint8_t value = 0xff;
        if(map.values[i * map.extents.x + j].type == Cell_Type::Water)
        {
          value = 0;
        }
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

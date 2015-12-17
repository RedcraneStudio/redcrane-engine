/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "grid.h"
#include "../common/log.h"

#include <png.h>
namespace game { namespace water
{
  Water::Water(int64_t seed, Vec<int> grid_extents) noexcept
  {
    open_simplex_noise(seed, &osn);

    // We know what grid size we want.
    extents = grid_extents;

    // The normal map should match be four times as large as the grid.
    grid_extents *= 4;
    normalmap.allocate(grid_extents);

    // The heightmap should be a little bit bigger so that generating the
    // normalmap with a 3x3 sobol kernel works.
    grid_extents = {grid_extents.x + 2, grid_extents.y + 2};
    heightmap.allocate(grid_extents);
  }
  Water::~Water() noexcept
  {
    open_simplex_noise_free(osn);
  }

  template <class T>
  float apply_sobel_kernel_x(Vec<int> pt, Value_Map<T> const& map,
                             float scale = 1.0f) noexcept
  {
    // Code derived from: http://stackoverflow.com/questions/5281261
    // "Generating a normal map from a height map?"

    auto s_2 = map.at({pt.x + 1, pt.y + 1});
    auto s_0 = map.at({pt.x - 1, pt.y + 1});
    auto s_5 = map.at({pt.x + 1, pt.y});
    auto s_3 = map.at({pt.x - 1, pt.y});
    auto s_8 = map.at({pt.x + 1, pt.y - 1});
    auto s_6 = map.at({pt.x - 1, pt.y - 1});
    return scale * -(s_2 - s_0 + 2 * (s_5 - s_3) + s_8 - s_6);
  }
  template <class T>
  float apply_sobel_kernel_y(Vec<int> pt, Value_Map<T> const& map,
                             float scale = 1.0f) noexcept
  {
    auto s_6 = map.at({pt.x - 1, pt.y - 1});
    auto s_0 = map.at({pt.x - 1, pt.y + 1});
    auto s_7 = map.at({pt.x, pt.y - 1});
    auto s_1 = map.at({pt.x, pt.y + 1});
    auto s_8 = map.at({pt.x + 1, pt.y - 1});
    auto s_2 = map.at({pt.x + 1, pt.y + 1});
    return scale * -(s_6 - s_0 + 2 * (s_7 - s_1) + s_8 - s_2);
  }

  void gen_normalmap(Water& water, float scale) noexcept
  {
    // Assert the extents of the normalmap are expected? (in comparison to the
    // heightmap)

    for(int i = 0; i < water.normalmap.extents.y * water.normalmap.extents.x;
        ++i)
    {
      auto x = i % water.normalmap.extents.x;
      auto y = i / water.normalmap.extents.x;

      // If we are (0,0), for example, we should apply the sobol filter where
      // the center on the heightmap is (1,1).
      auto pt = Vec<int>{x + 1, y + 1};

      auto& dir = water.normalmap.at({x, y});
      dir.x = apply_sobel_kernel_x(pt, water.heightmap, scale);
      dir.z = apply_sobel_kernel_y(pt, water.heightmap, scale);
      dir.y = 1.0f;
      dir = glm::normalize(dir);
    }
  }

  void blit_normalmap(Texture& tex, Water& water) noexcept
  {
    auto extents = water.normalmap.extents;
    tex.allocate(water.normalmap.extents, Image_Format::Rgba);

    float* data = new float[extents.x * extents.y * 4];

    for(int i = 0; i < extents.y; ++i)
    {
      for(int j = 0; j < extents.x; ++j)
      {
        auto vec = water.normalmap.at({j, i});

        data[i * (extents.x * 4) + (j * 4) + 0] = vec.x;
        data[i * (extents.x * 4) + (j * 4) + 1] = vec.y;
        data[i * (extents.x * 4) + (j * 4) + 2] = vec.z;

        // The w component is always 0.0 because the normal is a vector, not
        // a position, obviously. It's necessary for some math done in the
        // shader.
        data[i * extents.x + (j * 4) + 3] = 0.0f;
      }
    }

    tex.blit_data(vol_from_extents(extents), data);

    delete[] data;
  }
  void blit_heightmap(Texture& tex, Water& water) noexcept
  {
    tex.allocate(water.heightmap.extents, Image_Format::Depth);
    auto vol = vol_from_extents(water.heightmap.extents);
    tex.blit_data(vol, water.heightmap.values);
  }

  template <class T, class F>
  static void write_png(std::string fn, Value_Map<T>& map, F fmt) noexcept
  {
    // All this code is pretty much based completely on the libpng manual so
    // check that out.

    std::FILE* fp = std::fopen(fn.data(), "wb");
    if(!fp)
    {
      log_w("Failed to open file '%'", fn);
      return;
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                  NULL, NULL, NULL);
    if(!png_ptr)
    {
      log_w("Failed to initialize libpng to write file '%'", fn);
      return;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr)
    {
      png_destroy_write_struct(&png_ptr, NULL);
      log_w("Failed to initialize png info to write '%'", fn);
      return;
    }

    if(setjmp(png_jmpbuf(png_ptr)))
    {
      png_destroy_write_struct(&png_ptr, &info_ptr);
      fclose(fp);

      log_w("libpng error for '%'", fn);
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
        auto color = fmt(map.at({i, j}));
        ptrs.back()[j * 3] = color.r * 0xff;
        ptrs.back()[j * 3 + 1] = color.g * 0xff;
        ptrs.back()[j * 3 + 2] = color.b * 0xff;
      }
    }

    png_set_rows(png_ptr, info_ptr, &ptrs[0]);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    for(auto* ptr : ptrs) delete ptr;

    png_destroy_write_struct(&png_ptr, &info_ptr);
  }

  struct Heightmap_Color
  {
    glm::vec3 operator()(float height) noexcept
    {
      return glm::vec3(height);
    }
  };

  void write_heightmap_png(std::string fn, Water& water) noexcept
  {
    write_png(fn, water.heightmap, Heightmap_Color{});
  }

  struct Normalmap_Color
  {
    glm::vec3 operator()(glm::vec3 const& normal) noexcept
    {
      return normal;
    }
  };

  void write_normalmap_png(std::string fn, Water& water) noexcept
  {
    write_png(fn, water.normalmap, Normalmap_Color{});
  }
} }

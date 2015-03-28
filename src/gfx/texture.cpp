/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "texture.h"
#include "../common/log.h"
#include <cstring>
#include <png.h>
namespace survive
{
  void error_fn(void* usr_data) noexcept
  {}

  Texture Texture::from_png_file(std::string filename) noexcept
  {
    std::FILE* fp = std::fopen(filename.c_str(), "rb");
    if(!fp)
    {
      log_w("Failed to open file '%'", filename);
      return Texture{};
    }

    constexpr int HEADER_READ = 7;
    unsigned char* const header = new unsigned char[HEADER_READ];
    fread(header, 1, HEADER_READ, fp);
    bool is_png = !png_sig_cmp(header, 0, HEADER_READ);
    if(!is_png)
    {
      log_w("File '%' not a png", filename);
      return Texture{};
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                 NULL, NULL, NULL);
    if(!png_ptr)
    {
      log_w("Failed to initialize png struct for '%'", filename);
      return Texture{};
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr)
    {
      png_destroy_read_struct(&png_ptr, NULL, NULL);
      log_w("Failed to initialize png info for '%'", filename);
      return Texture{};
    }

    png_infop end_info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr)
    {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      log_w("Failed to initialize png end info for '%'", filename);
      return Texture{};
    }

    if(setjmp(png_jmpbuf(png_ptr)))
    {
      png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
      fclose(fp);

      log_w("libpng error for '%'", filename);
      return Texture{};
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, HEADER_READ);

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_GRAY_TO_RGB, NULL);

    auto texture = Texture{};
    texture.w = png_get_image_width(png_ptr, info_ptr);
    texture.h = png_get_image_height(png_ptr, info_ptr);

    auto png_data = png_get_rows(png_ptr, info_ptr);
    auto pitch = png_get_rowbytes(png_ptr, info_ptr);

    auto format = png_get_color_type(png_ptr, info_ptr);

    auto bytes_per_pixel = 4;
    auto png_bytes_per_pixel = int{};
    if(format == PNG_COLOR_TYPE_RGB)
    {
      png_bytes_per_pixel = 3;
    }
    else if(format == PNG_COLOR_TYPE_RGBA)
    {
      png_bytes_per_pixel = 4;
    }
    texture.format = Texture_Format::RGBA;

    texture.data = new uint8_t[texture.w * texture.h * bytes_per_pixel];
    texture.pitch = texture.w * bytes_per_pixel;
    for(int i = 0; i < texture.h; ++i)
    {
      for(int j = 0; j < texture.w; ++j)
      {
        auto pos = texture.data + texture.pitch * i + j * bytes_per_pixel;

        auto dst_ptr = *(png_data + i);
        dst_ptr += j * png_bytes_per_pixel;

        std::memcpy(pos, dst_ptr, png_bytes_per_pixel);
        if(format == PNG_COLOR_TYPE_RGB)
        {
          // We don't have an alpha value, so fill in full opacity.
          pos[3] = 0xff;
        }
      }
    }

    // We copied the data, so just forget about png now.
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
    fclose(fp);

    return texture;
  }
  Texture::Texture(Texture&& tex) noexcept
  {
    w = tex.w;
    h = tex.h;

    data = tex.data;
    tex.data = nullptr;

    pitch = tex.pitch;
    format = tex.format;
  }
  Texture& Texture::operator=(Texture&& tex) noexcept
  {
    w = tex.w;
    h = tex.h;

    data = tex.data;
    tex.data = nullptr;

    pitch = tex.pitch;
    format = tex.format;

    return *this;
  }
  Texture::~Texture() noexcept
  {
    if(data) delete[] data;
  }
}

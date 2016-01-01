/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "texture_load.h"
#include "../../common/log.h"
#include <png.h>
#include <cstring>
namespace redc
{
  void error_fn(void*) noexcept {}

  Image load_png_data(std::string filename) noexcept
  {
    Image image;

    std::FILE* fp = std::fopen(filename.c_str(), "rb");
    if(!fp)
    {
      log_w("Failed to open file '%'", filename);
      return {};
    }

    constexpr int HEADER_READ = 7;
    unsigned char* const header = new unsigned char[HEADER_READ];
    fread(header, 1, HEADER_READ, fp);
    bool is_png = !png_sig_cmp(header, 0, HEADER_READ);
    if(!is_png)
    {
      log_w("File '%' not a png", filename);
      return {};
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                 NULL, NULL, NULL);
    if(!png_ptr)
    {
      log_w("Failed to initialize png struct for '%'", filename);
      return {};
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr)
    {
      png_destroy_read_struct(&png_ptr, NULL, NULL);
      log_w("Failed to initialize png info for '%'", filename);
      return {};
    }

    png_infop end_info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr)
    {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      log_w("Failed to initialize png end info for '%'", filename);
      return {};
    }

    if(setjmp(png_jmpbuf(png_ptr)))
    {
      png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
      fclose(fp);

      log_w("libpng error for '%'", filename);
      return {};
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, HEADER_READ);

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_GRAY_TO_RGB, NULL);

    image.extents.x = png_get_image_width(png_ptr, info_ptr);
    image.extents.y = png_get_image_height(png_ptr, info_ptr);

    auto png_data = png_get_rows(png_ptr, info_ptr);
    auto format = png_get_color_type(png_ptr, info_ptr);

    int png_bytes_per_pixel = 0;
    if(format == PNG_COLOR_TYPE_RGB)
    {
      png_bytes_per_pixel = 3;
    }
    else if(format == PNG_COLOR_TYPE_RGBA)
    {
      png_bytes_per_pixel = 4;
    }

    image.data.reserve(image.extents.x * image.extents.y);
    for(int i = 0; i < image.extents.y * image.extents.x; ++i)
    {
      // Find out our position.
      int x = i % image.extents.x;
      int y = i / image.extents.y;

      // Find where we are in png's data.
      auto dst_ptr = *(png_data + y);
      dst_ptr += x * png_bytes_per_pixel;

      Color c;
      c.r = dst_ptr[0];
      c.g = dst_ptr[1];
      c.b = dst_ptr[2];
      if(format == PNG_COLOR_TYPE_RGB)
      {
        // We don't have an alpha value, so fill in full opacity.
        c.a = 0xff;
      }
      else if(format == PNG_COLOR_TYPE_RGBA)
      {
        c.a = dst_ptr[3];
      }

      image.data.push_back(c);
    }

    // We copied the data, so just forget about png now.
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
    // Close the file of course.
    fclose(fp);

    return image;
  }
  void blit_image(Texture& t, Image const& img) noexcept
  {
    static_assert(sizeof(Color) == sizeof(unsigned char) * 4,
                  "Color structs must be stored contigously");

    t.allocate(img.extents, Image_Format::Rgba, Image_Type::Tex_2D);

    t.blit_tex2d_data(vol_from_extents(img.extents),
                      Data_Type::Unsigned_Byte, &img.data[0]);
  }
  void allocate_cube_map(Texture& t, Image const& img) noexcept
  {
    t.allocate(img.extents, Image_Format::Rgba, Image_Type::Cube_Map);
  }
  void blit_cube_map_face(Texture& t, Cube_Map_Texture const& side,
                          Image const& img) noexcept
  {
    static_assert(sizeof(Color) == sizeof(unsigned char) * 4,
                  "Color structs must be stored contigously");

    t.blit_cube_data(side, vol_from_extents(img.extents), Data_Type::Unsigned_Byte,
                     &img.data[0]);
  }
}

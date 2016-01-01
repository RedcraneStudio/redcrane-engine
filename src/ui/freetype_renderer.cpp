/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "freetype_renderer.h"

#include <string>

#include "../gfx/texture.h"
#include "../common/log.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H
namespace redc { namespace ui
{
  struct detail::Freetype_Impl
  {
    FT_Library library;
    FTC_Manager manager;
    FTC_CMapCache cmap_cache;
    FTC_ImageCache image_cache;
  };

  FT_Error faceid_callback(FTC_FaceID id, FT_Library lib, FT_Pointer,
                           FT_Face* face) noexcept
  {
    if(id != (FTC_FaceID) 1) return 1;
    FT_New_Face(lib, "/usr/share/fonts/TTF/OpenSans-Regular.ttf", 0, face);
    return 0;
  }

  Freetype_Renderer::Freetype_Renderer() noexcept
  {
    impl_ = new detail::Freetype_Impl;

    FT_Init_FreeType(&impl_->library);
    FTC_Manager_New(impl_->library, 1, 0, 0, faceid_callback, NULL,
                    &impl_->manager);
    FTC_CMapCache_New(impl_->manager, &impl_->cmap_cache);
    FTC_ImageCache_New(impl_->manager, &impl_->image_cache);
  }
  Freetype_Renderer::~Freetype_Renderer() noexcept
  {
    delete impl_;
  }

  FT_Glyph load_glyph_(detail::Freetype_Impl* impl, char c, int size) noexcept
  {
    auto gc = FTC_CMapCache_Lookup(impl->cmap_cache, (FTC_FaceID) 0x1, -1, c);

    FTC_ScalerRec scaler;
    scaler.face_id = (FTC_FaceID) 0x1;
    scaler.pixel = 0; scaler.width = size * 72;  scaler.height = size * 72;
    scaler.x_res = 72; scaler.y_res = 72;

    // Look up our glyph.
    FT_Glyph glyph;
    FTC_ImageCache_LookupScaler(impl->image_cache, &scaler, FT_LOAD_RENDER,
                                gc, &glyph, NULL);
    return glyph;
  }

  Volume<int> glyph_vol_(detail::Freetype_Impl* impl, char c, int size,
                         Vec<int>& advance) noexcept
  {
    auto glyph = load_glyph_(impl, c, size);

    Volume<int> ret{};

    // Make sure we have a bitmap.
    if(glyph->format == FT_GLYPH_FORMAT_BITMAP)
    {
      FT_BitmapGlyph bitmap = (FT_BitmapGlyph) glyph;

      ret.pos.x = bitmap->left;
      ret.width = bitmap->bitmap.width;
      ret.pos.y = -bitmap->top;
      ret.height = bitmap->bitmap.rows;
    }

    advance.x = glyph->advance.x >> 16;
    advance.y = glyph->advance.y >> 16;

    return ret;
  }

  Volume<int> query_size_and_pen(detail::Freetype_Impl* impl,
                                 std::string const& text, int size) noexcept
  {
    int ideal_pen_x = 0, ideal_pen_y = 0;
    int pen_x = 0, pen_y = 0;
    int left = 0, right = 0, bottom = 0, top = 0;
    for(unsigned int i = 0; i < text.size(); ++i)
    {
      Vec<int> advance;
      auto vol = glyph_vol_(impl, text[i], size, advance);

      left = std::min(left, vol.pos.x);
      right = std::max(right, vol.pos.x + vol.width);
      bottom = std::max(bottom, vol.pos.y + vol.height);
      top = std::min(top, vol.pos.y);

      if(i == 0)
      {
        ideal_pen_x = std::max(0, left);
      }

      pen_x += advance.x;
      pen_y += advance.y;
    }

    // The bounds of ret are the extents of the image and the position is the
    // ideal pen position relative to it's origin.
    Volume<int> ret;

    // We need to export the ideal position of the pen.
    ret.width = std::max(pen_x, right - left);
    ret.height = std::max(pen_y, bottom - top);

    // Set ideal *starting* pen position.
    ideal_pen_y = std::max(0, -top);
    ret.pos = {ideal_pen_x, ideal_pen_y};

    return ret;
  }

  void Freetype_Renderer::text(std::string const& text, int size,
                               Texture& tb) noexcept
  {
    auto vol = query_size_and_pen(impl_, text, size);
    tb.allocate(vol_extents(vol));
    tb.fill_data(vol_from_extents(tb.allocated_extents()),
                 Color{0x00,0x00,0x00,0x00});

    for(char c : text)
    {
      blit_glyph(c, size, vol.pos.x, vol.pos.y, tb);
    }
  }

  Vec<int> Freetype_Renderer::
  query_size(std::string const& text, int size) noexcept
  {
    return vol_extents(query_size_and_pen(impl_, text, size));
  }

  void Freetype_Renderer::blit_glyph(char c, int size, int& pen_x, int& pen_y,
                                     Texture& tb) noexcept
  {
    auto glyph = load_glyph_(impl_, c, size);

    // Make sure we have a bitmap.
    if(glyph->format == FT_GLYPH_FORMAT_BITMAP)
    {
      FT_BitmapGlyph bitmap = (FT_BitmapGlyph) glyph;

      for(unsigned int i = 0; i < bitmap->bitmap.rows; ++i)
      {
        // Make a buffer.
        auto buf = new Color[bitmap->bitmap.width];

        for(int j = 0; (unsigned int) j < bitmap->bitmap.width; ++j)
        {
          // Only use our column, our row doesn't matter because we are doing
          // this row by row.
          auto& col = buf[j];

          // Expand data to colors.
          auto byte = bitmap->bitmap.buffer[i * bitmap->bitmap.pitch + j];
          col.r = 0xff;
          col.g = 0xff;
          col.b = 0xff;
          col.a = byte;

          // Configure these alpha-modes
          //if(byte == 0x00) col.a = 0x00;
          //else col.a = 0xff;

        }
        // Blit the row using our pen position.
        tb.blit_data({{pen_x + bitmap->left, pen_y - bitmap->top + (int) i},
                      (int) bitmap->bitmap.width, 1}, buf);
        delete[] buf;
      }
    }

    // Advance the pen for the caller.
    pen_x += glyph->advance.x >> 16;
    pen_y += glyph->advance.y >> 16;
  }
} }

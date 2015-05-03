/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "ifont_renderer.h"
namespace game { namespace ui
{
  namespace detail { struct Freetype_Impl; }

  struct Freetype_Renderer : public IFont_Renderer
  {
    Freetype_Renderer() noexcept;
    virtual ~Freetype_Renderer() noexcept;

    void text(std::string const&, int size, Texture& tb) noexcept override;
    Vec<int> query_size(std::string const&, int size) noexcept override;
  private:
    void blit_glyph(char c, int size, int& pen_x, int& pen_y,
                    Texture& tb) noexcept;

    detail::Freetype_Impl* impl_;
  };
} }

/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../common/vec.h"
#include "../common/volume.h"
#include "../common/color.h"
#include "../common/texture.h"

#include <memory>
namespace game { namespace ui
{
  // Pronounced: ui renderer interface
  struct Renderer
  {
    virtual ~Renderer() noexcept {}

    virtual void set_draw_color(Color const&) noexcept = 0;

    virtual void draw_rect(Volume<int> const&) noexcept = 0;
    virtual void fill_rect(Volume<int> const&) noexcept = 0;

    virtual std::unique_ptr<Texture> make_texture() noexcept = 0;

    inline void draw_texture(Volume<int> const& dst, Texture& tex) noexcept;
    virtual void draw_texture(Volume<int> const& dst, Texture& tex,
                              Volume<int> const& src) noexcept = 0;

    virtual void begin_draw() noexcept = 0;
    virtual void end_draw() noexcept = 0;
  };

  inline void Renderer::
  draw_texture(Volume<int> const& dst, Texture& tex) noexcept
  {
    auto src = vol_from_extents(tex.allocated_extents());
    draw_texture(dst, tex, src);
  }

  struct Draw_Scoped_Lock
  {
    inline Draw_Scoped_Lock(Renderer& r) noexcept;
    inline ~Draw_Scoped_Lock() noexcept;
    Renderer& renderer;
  };

  Draw_Scoped_Lock::Draw_Scoped_Lock(Renderer& r) noexcept : renderer(r)
  {
    renderer.begin_draw();
  }
  Draw_Scoped_Lock::~Draw_Scoped_Lock() noexcept
  {
    renderer.end_draw();
  }
} }

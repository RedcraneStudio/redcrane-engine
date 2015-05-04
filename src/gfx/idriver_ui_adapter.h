/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "idriver.h"
#include "../ui/renderer.h"
#include "../common/software_mesh.h"
namespace game { namespace gfx
{
  struct IDriver_UI_Adapter : public ui::Renderer
  {
    IDriver_UI_Adapter(IDriver& d) noexcept;

    virtual ~IDriver_UI_Adapter() noexcept {}

    void set_draw_color(Color const&) noexcept override;

    void draw_rect(Volume<int> const&) noexcept override;
    void fill_rect(Volume<int> const&) noexcept override;

    std::unique_ptr<Texture> make_texture() noexcept override;

    void draw_texture(Volume<int> const& dst, Texture& tex,
                      Volume<int> const& src) noexcept override;

    void begin_draw() noexcept override;
    void end_draw() noexcept override;

  private:
    IDriver* d_;

    Color dif_ = colors::white;

    std::unique_ptr<Texture> white_texture_;

    Software_Mesh filled_rect_;
    Software_Mesh lines_rect_;

    void set_rect_(Mesh& rect, Volume<int> vol) noexcept;

    void set_rect_(Mesh& rect, Volume<int> vol,
                   Volume<int> tex_v) const noexcept;
  };
} }

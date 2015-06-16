/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "idriver.h"
#include "../ui/renderer.h"
#include "support/scoped_shader_lock.h"
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

    std::unique_ptr<Shader_Push_Lock> shader_lock_;
    std::unique_ptr<Shader> hud_shader_;

    std::unique_ptr<Texture> white_texture_;

    std::unique_ptr<Mesh> mesh_;

    unsigned int pos_buf_;
    unsigned int tex_buf_;
    unsigned int col_buf_;

    unsigned int offset_ = 0;

    unsigned int pos_pos_ = 0;
    unsigned int tex_pos_ = 0;
    unsigned int col_pos_ = 0;

    Color cur_dif_ = colors::white;

    enum class Render_Type { Fill, Draw };
    struct Rectangle
    {
      Render_Type type;
      unsigned int offset;
      unsigned int count;

      Texture* texture = nullptr;
    };

    std::vector<Rectangle> to_draw_;

    void set_rect_(Mesh& rect, Volume<int> vol) noexcept;

    void set_rect_(Mesh& rect, Volume<int> vol,
                   Volume<int> tex_v, Vec<int> tex_max) const noexcept;
  };
} }

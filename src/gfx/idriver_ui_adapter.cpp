/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "idriver_ui_adapter.h"
#include <glm/gtc/matrix_transform.hpp>
namespace game { namespace gfx
{
  IDriver_UI_Adapter::IDriver_UI_Adapter(IDriver& d) noexcept
    : d_(&d), hud_shader_(std::move(d.make_shader_repr())),
      mesh_(d.make_mesh_repr())
  {
    // Somehow manage these resources in a better way?
    // Or otherwise declare this data in json?
    hud_shader_->load_vertex_part("shader/hud/v");
    hud_shader_->load_fragment_part("shader/hud/f");

    hud_shader_->set_projection_name("ortho");
    hud_shader_->set_diffuse_name("dif");
    hud_shader_->set_sampler_name("tex");

    hud_shader_->set_sampler(0);

    // Prepare an all white texture so we can just change the diffuse color.
    // We can change this when the driver and the shader become more
    // distinguished, right now I'm not really sure how this can be done so
    // this is the simplest option, without adding another shader/program.
    white_texture_ = d_->make_texture_repr();
    white_texture_->allocate(Vec<int>{1,1});
    white_texture_->blit_data({{0,0}, 1, 1}, &colors::white);

    // Initialize two buffers for our mesh. One for positional data and the
    // other for texture coordinates.

    // We don't know how much data we actually need for now, so for now
    // allocate four kilobytes and hope for the best.
    pos_buf_ = mesh_->allocate_buffer(4096, Usage_Hint::Draw,
                                      Upload_Hint::Stream);
    mesh_->format_buffer(pos_buf_, 0, 2, Buffer_Format::Float, 0, 0);
    mesh_->enable_vertex_attrib(0);

    tex_buf_ = mesh_->allocate_buffer(4096, Usage_Hint::Draw,
                                      Upload_Hint::Stream);
    mesh_->format_buffer(tex_buf_, 1, 2, Buffer_Format::Float, 0, 0);
    mesh_->enable_vertex_attrib(1);

    col_buf_ = mesh_->allocate_buffer(4096, Usage_Hint::Draw,
                                      Upload_Hint::Stream);
    mesh_->format_buffer(col_buf_, 2, 4, Buffer_Format::Float, 0, 0);
    mesh_->enable_vertex_attrib(2);
  }

  void IDriver_UI_Adapter::set_draw_color(Color const& c) noexcept
  {
    cur_dif_ = c;
  }

#define EXPAND_VEC4(v) v.x, v.y, v.z, v.w
  void IDriver_UI_Adapter::draw_rect(Volume<int> const& vol) noexcept
  {
    std::array<float, 2*8> positions =
    {
      (float) vol.pos.x,             (float) vol.pos.y,
      (float) vol.pos.x + vol.width, (float) vol.pos.y,

      (float) vol.pos.x + vol.width, (float) vol.pos.y,
      (float) vol.pos.x + vol.width, (float) vol.pos.y + vol.height,

      (float) vol.pos.x + vol.width, (float) vol.pos.y + vol.height,
      (float) vol.pos.x,             (float) vol.pos.y + vol.height,

      (float) vol.pos.x,             (float) vol.pos.y + vol.height,
      (float) vol.pos.x,             (float) vol.pos.y,
    };
    std::array<float, 2*8> tex_coords =
    {
      .5, .5,
      .5, .5,
      .5, .5,
      .5, .5,

      .5, .5,
      .5, .5,
      .5, .5,
      .5, .5
    };

    auto cur_dif_vec = c_to_vec4(cur_dif_);
    std::array<float, 4*8> colors =
    {
      EXPAND_VEC4(cur_dif_vec),
      EXPAND_VEC4(cur_dif_vec),
      EXPAND_VEC4(cur_dif_vec),
      EXPAND_VEC4(cur_dif_vec),

      EXPAND_VEC4(cur_dif_vec),
      EXPAND_VEC4(cur_dif_vec),
      EXPAND_VEC4(cur_dif_vec),
      EXPAND_VEC4(cur_dif_vec)
    };

    Shape rect;

    // Set fields and populate (append) the buffer.
    rect.type = Render_Type::Draw;
    rect.offset = offset_;
    rect.count = 8;

    auto count_bytes = sizeof(float) * positions.size();
    mesh_->buffer_data(pos_buf_, pos_pos_, count_bytes, &positions[0]);
    pos_pos_ += count_bytes;

    count_bytes = sizeof(float) * tex_coords.size();
    mesh_->buffer_data(tex_buf_, tex_pos_, count_bytes, &tex_coords[0]);
    tex_pos_ += count_bytes;

    count_bytes = sizeof(float) * colors.size();
    mesh_->buffer_data(col_buf_, col_pos_, count_bytes, &colors[0]);
    col_pos_ += count_bytes;

    offset_ += rect.count;

    to_draw_.push_back(rect);
  }

  void IDriver_UI_Adapter::fill_rect(Volume<int> const& vol) noexcept
  {
    std::array<float, 2*6> positions =
    {
      (float) vol.pos.x,             (float) vol.pos.y,
      (float) vol.pos.x + vol.width, (float) vol.pos.y + vol.height,
      (float) vol.pos.x + vol.width, (float) vol.pos.y,

      (float) vol.pos.x,             (float) vol.pos.y,
      (float) vol.pos.x,             (float) vol.pos.y + vol.height,
      (float) vol.pos.x + vol.width, (float) vol.pos.y + vol.height
    };
    std::array<float, 2*6> tex_coords =
    {
      .5, .5,
      .5, .5,
      .5, .5,

      .5, .5,
      .5, .5,
      .5, .5
    };

    auto cur_dif_vec = c_to_vec4(cur_dif_);
    std::array<float, 4*6> colors =
    {
      EXPAND_VEC4(cur_dif_vec),
      EXPAND_VEC4(cur_dif_vec),
      EXPAND_VEC4(cur_dif_vec),

      EXPAND_VEC4(cur_dif_vec),
      EXPAND_VEC4(cur_dif_vec),
      EXPAND_VEC4(cur_dif_vec)
    };

    Shape rect;

    // Set fields and populate (append) the buffer.
    rect.type = Render_Type::Fill;
    rect.offset = offset_;
    rect.count = 6;
    rect.texture = white_texture_.get();

    // Append positions, texture coordinates, and vertex colors.
    auto count_bytes = sizeof(float) * positions.size();
    mesh_->buffer_data(pos_buf_, pos_pos_, count_bytes, &positions[0]);
    pos_pos_ += count_bytes;

    count_bytes = sizeof(float) * tex_coords.size();
    mesh_->buffer_data(tex_buf_, tex_pos_, count_bytes, &tex_coords[0]);
    tex_pos_ += count_bytes;

    count_bytes = sizeof(float) * colors.size();
    mesh_->buffer_data(col_buf_, col_pos_, count_bytes, &colors[0]);
    col_pos_ += count_bytes;

    offset_ += rect.count;

    to_draw_.push_back(rect);
  }

  void IDriver_UI_Adapter::fill_circle(Circle<int> circle, int sbdivs) noexcept
  {
    auto radius = circle.radius;
    auto center = circle.center;

    auto cur_dif_vec = c_to_vec4(cur_dif_);
    std::vector<float> colors;
    colors.push_back(cur_dif_vec.r);
    colors.push_back(cur_dif_vec.g);
    colors.push_back(cur_dif_vec.b);
    colors.push_back(cur_dif_vec.a);

    std::vector<float> tex_coords;
    tex_coords.push_back(0.5);
    tex_coords.push_back(0.5);

    std::vector<float> positions;
    positions.push_back(center.x);
    positions.push_back(center.y);

    auto angle = 2.0f * M_PI / sbdivs;
    for(int i = 0; i <= sbdivs; ++i)
    {
      // Negate sin so that we get a CCW winding order after the y-flip.
      positions.push_back(glm::cos(angle * i) * radius + center.x);
      positions.push_back(-glm::sin(angle * i) * radius + center.y);

      colors.push_back(cur_dif_vec.r);
      colors.push_back(cur_dif_vec.g);
      colors.push_back(cur_dif_vec.b);
      colors.push_back(cur_dif_vec.a);

      tex_coords.push_back(0.5);
      tex_coords.push_back(0.5);
    }

    Shape circle_render;

    // Set fields and populate (append) the buffer.
    circle_render.type = Render_Type::Triangle_Fan;
    circle_render.offset = offset_;
    circle_render.count = sbdivs + 1 + 1;
    circle_render.texture = white_texture_.get();

    auto count_bytes = sizeof(float) * positions.size();
    mesh_->buffer_data(pos_buf_, pos_pos_, count_bytes, &positions[0]);
    pos_pos_ += count_bytes;

    count_bytes = sizeof(float) * tex_coords.size();
    mesh_->buffer_data(tex_buf_, tex_pos_, count_bytes, &tex_coords[0]);
    tex_pos_ += count_bytes;

    count_bytes = sizeof(float) * colors.size();
    mesh_->buffer_data(col_buf_, col_pos_, count_bytes, &colors[0]);
    col_pos_ += count_bytes;

    offset_ += circle_render.count;

    to_draw_.push_back(circle_render);
  }
  void IDriver_UI_Adapter::draw_circle(Circle<int> circle, int sbdivs) noexcept
  {
    auto radius = circle.radius;
    auto center = circle.center;

    auto cur_dif_vec = c_to_vec4(cur_dif_);
    std::vector<float> colors;
    std::vector<float> tex_coords;

    std::vector<float> positions;

    auto angle = 2.0f * M_PI / sbdivs;
    for(int i = 0; i < sbdivs; ++i)
    {
      positions.push_back(glm::cos(angle * i) * radius + center.x);
      positions.push_back(glm::sin(angle * i) * radius + center.y);

      positions.push_back(glm::cos(angle * (i + 1)) * radius + center.x);
      positions.push_back(glm::sin(angle * (i + 1)) * radius + center.y);

      colors.push_back(cur_dif_vec.r);
      colors.push_back(cur_dif_vec.g);
      colors.push_back(cur_dif_vec.b);
      colors.push_back(cur_dif_vec.a);

      colors.push_back(cur_dif_vec.r);
      colors.push_back(cur_dif_vec.g);
      colors.push_back(cur_dif_vec.b);
      colors.push_back(cur_dif_vec.a);

      tex_coords.push_back(0.5);
      tex_coords.push_back(0.5);

      tex_coords.push_back(0.5);
      tex_coords.push_back(0.5);
    }

    Shape circle_render;

    // Set fields and populate (append) the buffer.
    circle_render.type = Render_Type::Draw;
    circle_render.offset = offset_;
    circle_render.count = sbdivs * 2;
    circle_render.texture = white_texture_.get();

    auto count_bytes = sizeof(float) * positions.size();
    mesh_->buffer_data(pos_buf_, pos_pos_, count_bytes, &positions[0]);
    pos_pos_ += count_bytes;

    count_bytes = sizeof(float) * tex_coords.size();
    mesh_->buffer_data(tex_buf_, tex_pos_, count_bytes, &tex_coords[0]);
    tex_pos_ += count_bytes;

    count_bytes = sizeof(float) * colors.size();
    mesh_->buffer_data(col_buf_, col_pos_, count_bytes, &colors[0]);
    col_pos_ += count_bytes;

    offset_ += circle_render.count;

    to_draw_.push_back(circle_render);
  }

  void IDriver_UI_Adapter::draw_line(Vec<int> p1, Vec<int> p2) noexcept
  {
    std::array<float, 2*2> positions =
    {
      (float) p1.x, (float) p1.y, (float) p2.x, (float) p2.y
    };
    std::array<float, 2*2> tex_coords =
    {
      .5, .5, .5, .5,
    };

    auto cur_dif_vec = c_to_vec4(cur_dif_);
    std::array<float, 4*2> colors =
    {
      EXPAND_VEC4(cur_dif_vec), EXPAND_VEC4(cur_dif_vec)
    };

    Shape line;

    // Set fields and populate (append) the buffer.
    line.type = Render_Type::Draw;
    line.offset = offset_;
    line.count = 2;
    line.texture = white_texture_.get();

    auto count_bytes = sizeof(float) * positions.size();
    mesh_->buffer_data(pos_buf_, pos_pos_, count_bytes, &positions[0]);
    pos_pos_ += count_bytes;

    count_bytes = sizeof(float) * tex_coords.size();
    mesh_->buffer_data(tex_buf_, tex_pos_, count_bytes, &tex_coords[0]);
    tex_pos_ += count_bytes;

    count_bytes = sizeof(float) * colors.size();
    mesh_->buffer_data(col_buf_, col_pos_, count_bytes, &colors[0]);
    col_pos_ += count_bytes;

    offset_ += line.count;

    to_draw_.push_back(line);
  }

  std::unique_ptr<Texture> IDriver_UI_Adapter::make_texture() noexcept
  {
    return d_->make_texture_repr();
  }

  void IDriver_UI_Adapter::draw_texture(Volume<int> const& dst, Texture& tex,
                                        Volume<int> const& src) noexcept
  {
    std::array<float, 2*6> positions =
    {
      (float) dst.pos.x,             (float) dst.pos.y,
      (float) dst.pos.x + dst.width, (float) dst.pos.y + dst.height,
      (float) dst.pos.x + dst.width, (float) dst.pos.y,

      (float) dst.pos.x,             (float) dst.pos.y,
      (float) dst.pos.x,             (float) dst.pos.y + dst.height,
      (float) dst.pos.x + dst.width, (float) dst.pos.y + dst.height
    };

    auto tex_e = tex.allocated_extents();

    auto tex_src = volume_cast<float>(src);

    tex_src.pos.x /= (float) tex_e.x;
    tex_src.width /= (float) tex_e.x;

    tex_src.pos.y /= (float) tex_e.y;
    tex_src.height /= (float) tex_e.y;

    std::array<float, 2*6> tex_coords =
    {
      tex_src.pos.x,                 tex_src.pos.y + tex_src.height,
      tex_src.pos.x + tex_src.width, tex_src.pos.y,
      tex_src.pos.x + tex_src.width, tex_src.pos.y + tex_src.height,

      tex_src.pos.x,                 tex_src.pos.y + tex_src.height,
      tex_src.pos.x,                 tex_src.pos.y,
      tex_src.pos.x + tex_src.width, tex_src.pos.y
    };

    auto cur_dif_vec = c_to_vec4(cur_dif_);
    std::array<float, 4*6> colors =
    {
      EXPAND_VEC4(cur_dif_vec),
      EXPAND_VEC4(cur_dif_vec),
      EXPAND_VEC4(cur_dif_vec),

      EXPAND_VEC4(cur_dif_vec),
      EXPAND_VEC4(cur_dif_vec),
      EXPAND_VEC4(cur_dif_vec)
    };

    Shape rect;

    // Set fields and populate (append) the buffer.
    rect.type = Render_Type::Fill;
    rect.offset = offset_;
    rect.count = 6;
    rect.texture = &tex;

    auto count_bytes = sizeof(float) * positions.size();
    mesh_->buffer_data(pos_buf_, pos_pos_, count_bytes, &positions[0]);
    pos_pos_ += count_bytes;

    count_bytes = sizeof(float) * tex_coords.size();
    mesh_->buffer_data(tex_buf_, tex_pos_, count_bytes, &tex_coords[0]);
    tex_pos_ += count_bytes;

    count_bytes = sizeof(float) * colors.size();
    mesh_->buffer_data(col_buf_, col_pos_, count_bytes, &colors[0]);
    col_pos_ += count_bytes;

    offset_ += rect.count;

    to_draw_.push_back(rect);
  }

  void IDriver_UI_Adapter::begin_draw() noexcept { }
  void IDriver_UI_Adapter::end_draw() noexcept
  {
    using SPL = Shader_Push_Lock;
    shader_lock_ = std::make_unique<SPL>(push_shader(*hud_shader_, *d_));

    auto size = vec_cast<float>(d_->window_extents());
    hud_shader_->set_projection(glm::ortho(0.0f, size.x, size.y, 0.0f,
                               -1.0f, 1.0f));

    d_->depth_test(false);
    d_->blending(true);

    for(Shape const& r : to_draw_)
    {
      switch(r.type)
      {
        case Render_Type::Draw:
          mesh_->set_primitive_type(Primitive_Type::Line);
          break;
        case Render_Type::Fill:
          mesh_->set_primitive_type(Primitive_Type::Triangle);
          break;
        case Render_Type::Triangle_Fan:
          mesh_->set_primitive_type(Primitive_Type::Triangle_Fan);
          break;
      }

      d_->bind_texture(*r.texture, 0);
      mesh_->draw_arrays(r.offset, r.count);
    }

    // For now, reset the mesh (or pretend to) until we can figure out a way to
    // check if anything is going to change. If nothing has changed in the ui
    // there is no reason to completely reset, after all.

    offset_ = 0;
    pos_pos_ = 0;
    tex_pos_ = 0;
    col_pos_ = 0;
    to_draw_.clear();

    d_->depth_test(true);
    d_->blending(false);

    shader_lock_.reset();
  }
} }

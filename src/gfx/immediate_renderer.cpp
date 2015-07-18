/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "immediate_renderer.h"
#include "support/scoped_shader_lock.h"
namespace game { namespace gfx
{
  Immediate_Renderer::Immediate_Renderer(IDriver& d) noexcept : d_(&d)
  {
    shader_ = d_->make_shader_repr();
    shader_->load_vertex_part("shader/debug/v");
    shader_->load_fragment_part("shader/debug/f");

    shader_->set_view_name("view");
    shader_->set_projection_name("proj");

    shader_->set_diffuse_name("color");

    mesh_ = d_->make_mesh_repr();

    pos_buf_ = mesh_->allocate_buffer(4096, Usage_Hint::Draw,
                                      Upload_Hint::Stream);
    mesh_->format_buffer(pos_buf_, 0, 3, Buffer_Format::Float, 0, 0);
    mesh_->enable_vertex_attrib(0);

    mesh_->set_primitive_type(Primitive_Type::Line);

    pos_pos_ = 0;
  }

  void Immediate_Renderer::set_draw_color(Color const& color) noexcept
  {
    cur_dif_ = color;
  }

  void Immediate_Renderer::draw_aabb(AABB const& aabb) noexcept
  {
    // Two points per line; Twelve lines; Three components per vertex.
    std::array<float, 3*12*2> positions =
    {
      // Bottom four lines
      aabb.min.x,              aabb.min.y, aabb.min.z,
      aabb.min.x + aabb.width, aabb.min.y, aabb.min.z,

      aabb.min.x + aabb.width, aabb.min.y, aabb.min.z,
      aabb.min.x + aabb.width, aabb.min.y, aabb.min.z + aabb.depth,

      aabb.min.x + aabb.width, aabb.min.y, aabb.min.z + aabb.depth,
      aabb.min.x,              aabb.min.y, aabb.min.z + aabb.depth,

      aabb.min.x,              aabb.min.y, aabb.min.z + aabb.depth,
      aabb.min.x,              aabb.min.y, aabb.min.z,

      // Top four lines
      aabb.min.x,              aabb.min.y+aabb.height, aabb.min.z,
      aabb.min.x + aabb.width, aabb.min.y+aabb.height, aabb.min.z,

      aabb.min.x + aabb.width, aabb.min.y+aabb.height, aabb.min.z,
      aabb.min.x + aabb.width, aabb.min.y+aabb.height, aabb.min.z + aabb.depth,

      aabb.min.x + aabb.width, aabb.min.y+aabb.height, aabb.min.z + aabb.depth,
      aabb.min.x,              aabb.min.y+aabb.height, aabb.min.z + aabb.depth,

      aabb.min.x,              aabb.min.y+aabb.height, aabb.min.z + aabb.depth,
      aabb.min.x,              aabb.min.y+aabb.height, aabb.min.z,

      // Lines connecting bottom and top
      aabb.min.x,              aabb.min.y,             aabb.min.z,
      aabb.min.x,              aabb.min.y+aabb.height, aabb.min.z,

      aabb.min.x + aabb.width, aabb.min.y,             aabb.min.z,
      aabb.min.x + aabb.width, aabb.min.y+aabb.height, aabb.min.z,

      aabb.min.x + aabb.width, aabb.min.y,             aabb.min.z + aabb.depth,
      aabb.min.x + aabb.width, aabb.min.y+aabb.height, aabb.min.z + aabb.depth,

      aabb.min.x,              aabb.min.y,             aabb.min.z + aabb.depth,
      aabb.min.x,              aabb.min.y+aabb.height, aabb.min.z + aabb.depth,
    };

    auto count_bytes = sizeof(float) * positions.size();
    mesh_->buffer_data(pos_buf_, pos_pos_, count_bytes, &positions[0]);
    pos_pos_ += count_bytes;
  }

  void Immediate_Renderer::reset() noexcept
  {
    pos_pos_ = 0;
  }
  void Immediate_Renderer::render(Camera const& cam) noexcept
  {
    auto shader_lock = push_shader(*shader_, *d_);
    shader_->set_diffuse(cur_dif_);
    shader_->set_projection(camera_proj_matrix(cam));
    shader_->set_view(camera_view_matrix(cam));
    mesh_->draw_arrays(0, pos_pos_);
  }
} }

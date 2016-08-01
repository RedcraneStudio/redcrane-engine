/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "immediate_renderer.h"
#include "support/scoped_shader_lock.h"
namespace redc { namespace gfx
{
  Immediate_Renderer::Immediate_Renderer(IDriver& d) noexcept : d_(&d)
  {
    // Create our shader
    shader_ = d_->make_shader_repr();

    // Load it from files
    load_vertex_file(*shader_, "shader/debug/vs.glsl");
    load_fragment_file(*shader_, "shader/debug/fs.glsl");

    // Link
    shader_->link();

    using namespace tags;
    // Tag
    shader_->set_var_tag(view_tag, "view");
    shader_->set_var_tag(proj_tag, "proj");

    shader_->set_var_tag(diffuse_tag, "color");

    // Make our mesh
    mesh_ = d_->make_mesh_repr();

    buf_size_ = 50000;
    pos_buf_ = mesh_->allocate_buffer(buf_size_, Usage_Hint::Draw,
                                      Upload_Hint::Stream);
    mesh_->format_buffer(pos_buf_, 0, 3, Data_Type::Float, 0, 0);
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

    auto offset = pos_pos_ * 3 * sizeof(float); // Each indice = 3 floats.
    auto count = positions.size() * sizeof(float); // Size in bytes

    if(buf_size_ < offset + count) return; // Overflow!

    mesh_->buffer_data(pos_buf_, offset, count, &positions[0]);
    pos_pos_ += 24;
  }
  void Immediate_Renderer::draw_line(glm::vec3 const& pt1,
                                     glm::vec3 const& pt2) noexcept
  {
    std::array<float, 3*2> positions =
    {
      pt1.x, pt1.y, pt1.z,
      pt2.x, pt2.y, pt2.z
    };

    auto offset = pos_pos_ * 3 * sizeof(float);
    auto count = positions.size() * sizeof(float);

    if(buf_size_ < offset + count) return; // Overflow!

    mesh_->buffer_data(pos_buf_, offset, count, &positions[0]);
    pos_pos_ += 2;
  }

  void Immediate_Renderer::reset() noexcept
  {
    pos_pos_ = 0;
  }
  void Immediate_Renderer::render(Camera const& cam) noexcept
  {
    using namespace gfx::tags;

    auto shader_lock = push_shader(*shader_, *d_);
    shader_->set_color(diffuse_tag, cur_dif_);
    shader_->set_mat4(proj_tag, camera_proj_matrix(cam));
    shader_->set_mat4(view_tag, camera_view_matrix(cam));
    mesh_->draw_arrays(0, pos_pos_);
  }
} }

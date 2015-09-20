/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#include "render_normals.h"
namespace game { namespace gfx
{
  void render_normals(Immediate_Renderer& r, Indexed_Mesh_Data const& m,
                      glm::mat4 const& model) noexcept
  {
    for(auto i : m.elements)
    {
      auto vertex = m.vertices[i];

      auto position = model * glm::vec4(vertex.position, 1.0f);
      auto normal = model * glm::vec4(vertex.normal, 0.0f);

      r.draw_line(glm::vec3(position), glm::vec3(position + normal));
    }
  }
  void render_normals(Immediate_Renderer& r, Ordered_Mesh_Data const& m,
                      glm::mat4 const& model) noexcept
  {
    for(auto vertex : m.vertices)
    {
      auto position = model * glm::vec4(vertex.position, 1.0f);
      auto normal = model * glm::vec4(vertex.normal, 0.0f);
      r.draw_line(glm::vec3(position), glm::vec3(position + normal));
    }
  }
  void render_normals(Immediate_Renderer& r,
                      Indexed_Split_Mesh_Data const& m,
                      glm::mat4 const& model) noexcept
  {
    for(auto vert_ref : m.indices)
    {
      auto position = model * glm::vec4(m.positions[*vert_ref.position], 1.0f);
      auto normal = model * glm::vec4(m.normals[*vert_ref.normal], 0.0f);
      r.draw_line(glm::vec3(position), glm::vec3(position + normal));
    }
  }
  void render_normals(Immediate_Renderer& r,
                      Ordered_Split_Mesh_Data const& m,
                      glm::mat4 const& model) noexcept
  {
    auto size = std::min(m.positions.size(), m.normals.size());
    for(size_t i = 0; i < size; ++i)
    {
      auto position = model * glm::vec4(m.positions[i], 1.0f);
      auto normal = model * glm::vec4(m.normals[i], 0.0f);
      r.draw_line(glm::vec3(position), glm::vec3(position + normal));
    }
  }
} }

/*
 * Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
 * All rights reserved.
 */
#pragma once
#include "../immediate_renderer.h"
#include "../mesh_data.h"
namespace redc { namespace gfx
{
  void render_normals(Immediate_Renderer&, Indexed_Mesh_Data const&,
                      glm::mat4 const&) noexcept;
  void render_normals(Immediate_Renderer&, Ordered_Mesh_Data const&,
                      glm::mat4 const&) noexcept;
  void render_normals(Immediate_Renderer&, Indexed_Split_Mesh_Data const&,
                      glm::mat4 const&) noexcept;
  void render_normals(Immediate_Renderer&, Ordered_Split_Mesh_Data const&,
                      glm::mat4 const&) noexcept;
} }

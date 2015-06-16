/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "driver.h"
#include "mesh.h"
#include "texture.h"
namespace game { namespace gfx { namespace null
{
  std::unique_ptr<Shader> Driver::make_shader_repr() noexcept
  {
    return std::make_unique<Shader>();
  }
  void Driver::use_shader(Shader& s) noexcept
  {
    cur_shader_ = &s;
  }
  Shader* Driver::active_shader() const noexcept
  {
    return cur_shader_;
  }

  std::unique_ptr<Mesh> Driver::make_mesh_repr() noexcept
  {
    return std::make_unique<Null_Mesh>();
  }

  std::unique_ptr<Texture> Driver::make_texture_repr() noexcept
  {
    return std::make_unique<Null_Texture>();
  }
} } }

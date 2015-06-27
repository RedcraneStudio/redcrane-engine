/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <glm/glm.hpp>
#include "../../common/vec.h"
namespace game { namespace gfx
{
  struct IDriver; struct Camera;

  glm::vec3 unproject_screen(gfx::IDriver& d, gfx::Camera& cam,
                             glm::mat4 const& m, Vec<int> v) noexcept;
} }

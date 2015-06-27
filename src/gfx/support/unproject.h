/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <glm/glm.hpp>
#include "../../common/vec.h"

#include <boost/optional.hpp>
namespace game { namespace gfx
{
  struct IDriver; struct Camera;

  using boost::none;
  glm::vec3 unproject_screen(gfx::IDriver& d, gfx::Camera& cam,
                             glm::mat4 const& m, Vec<int> v,
                             boost::optional<float> depth = none) noexcept;
} }

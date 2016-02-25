/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../../common/vec.h"
#include "../../common/value_map.h"
#include "../mesh_data.h"
#include "../texture.h"
#include "../camera.h"
#include "../../common/plane.h"

extern "C"
{
  #include "opensimplex/open-simplex-noise.h"
}

namespace redc { namespace proj_grid
{
  std::vector<glm::vec3> find_visible(gfx::Camera const& cam, float y,
                                      float max_disp) noexcept;
  glm::mat4 build_projector(gfx::Camera cam, Plane const& p, float disp) noexcept;
  glm::mat4 build_min_max_mat(std::vector<glm::vec3>& intersections,
                             glm::mat4 const& projector,
                             Plane const& p) noexcept;
  std::vector<glm::vec3> project_grid(std::vector<glm::vec2> const& grid,
                                      glm::mat4 projector,
                                      Plane const& p) noexcept;
  std::vector<glm::vec2> gen_grid(const unsigned int width) noexcept;
} }

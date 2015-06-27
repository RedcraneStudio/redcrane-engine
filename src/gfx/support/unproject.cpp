/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "unproject.h"

#include "../idriver.h"
#include "../camera.h"

#include <glm/gtc/matrix_transform.hpp>
namespace game { namespace gfx
{
  glm::vec3 unproject_screen(gfx::IDriver& d, gfx::Camera& cam,
                             glm::mat4 const& mod, Vec<int> v,
                             boost::optional<float> depth_in) noexcept
  {
    // Read pixel automatically inverts the y.
    float depth = 0.0f;
    if(depth_in)
    {
      depth = depth_in.value();
    }
    else
    {
      depth = d.read_pixel(gfx::Framebuffer::Depth, v);
    }

    // @ Refactor: Find out how to abstract the y-axis mouse-coordinate
    // inversion.
    glm::vec3 win((float) v.x, (float) d.window_extents().y - v.y, depth);

    glm::vec4 viewport(0, 0, d.window_extents().x, d.window_extents().y);

    auto obj_coord = glm::unProject(win, camera_view_matrix(cam) * mod,
                                    camera_proj_matrix(cam), viewport);

    return obj_coord;
  }
} }

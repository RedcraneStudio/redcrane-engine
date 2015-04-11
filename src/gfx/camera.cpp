/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
namespace strat
{
  glm::mat4 gfx::camera_view_matrix(Camera const& cam) noexcept
  {
    return glm::lookAt(cam.eye, cam.look, cam.up);
  }
  glm::mat4 gfx::camera_proj_matrix(Camera const& cam) noexcept
  {
    if(cam.projection_mode == Camera_Type::Orthographic)
    {
      return glm::ortho(cam.ortho.left, cam.ortho.right,
                        cam.ortho.bottom, cam.ortho.top,
                        cam.ortho.near, cam.ortho.far);
    }
    else if(cam.projection_mode == Camera_Type::Perspective)
    {
      return glm::perspective(cam.perspective.fov, cam.perspective.aspect,
                              cam.perspective.near, cam.perspective.far);
    }
    return glm::mat4(1.0);
  }
}

/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
namespace strat
{
  namespace gfx
  {
    glm::mat4 camera_view_matrix(Camera const& cam) noexcept
    {
      return glm::lookAt(cam.eye, cam.look, cam.up);
    }
    glm::mat4 camera_proj_matrix(Camera const& cam) noexcept
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
    Camera make_isometric_camera() noexcept
    {
      auto cam = Camera{};

      cam.projection_mode = Camera_Type::Orthographic;
      cam.ortho = Ortho_Cam_Params{-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f};
      cam.eye = glm::vec3(5, 10, 5);
      cam.look = glm::vec3(0, 0, 0);
      cam.up = glm::vec3(0, 1, 0);

      return cam;
    }
  }
}

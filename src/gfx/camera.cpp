/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "idriver.h"
namespace game
{
  namespace gfx
  {
    glm::mat4 camera_view_matrix(Camera const& cam) noexcept
    {
      if(cam.definition == Camera_Definition::Look_At)
      {
        return glm::lookAt(cam.look_at.eye, cam.look_at.look, cam.look_at.up);
      }
      else if(cam.definition == Camera_Definition::Pitch_Yaw_Pos)
      {
        // Do a translation, then the rotations.

        glm::mat4 ret(1.0f);
        ret = glm::rotate(ret, cam.fp.pitch, glm::vec3(1.0f, 0.0f, 0.0f));
        ret = glm::rotate(ret, cam.fp.yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        ret = glm::translate(ret, -cam.fp.pos);

        return ret;
      }
      return glm::mat4(1.0);
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

      cam.projection_mode = Camera_Type::Perspective;
      cam.perspective = Perspective_Cam_Params{glm::radians(50.0f),
                                               1.0f, .01f, 100.0f};

      cam.definition = Camera_Definition::Pitch_Yaw_Pos;
      cam.fp.pos = glm::vec3(5, 5, 5);
      cam.fp.yaw = glm::radians(-45.0f);
      cam.fp.pitch = glm::radians(45.0f);

      return cam;
    }
    Camera make_fps_camera() noexcept
    {
      auto cam = Camera{};

      cam.projection_mode = Camera_Type::Perspective;
      cam.perspective = Perspective_Cam_Params{glm::radians(50.0f),
                                               1.0f, .01f, 25.0f};

      cam.definition = Camera_Definition::Pitch_Yaw_Pos;
      cam.fp.pos = glm::vec3(0.0);
      cam.fp.yaw = 0.0f;
      cam.fp.pitch = 0.0f;
      return cam;
    }
    void use_camera(IDriver& driver, Camera const& cam) noexcept
    {
      driver.set_view(camera_view_matrix(cam));
      driver.set_projection(camera_proj_matrix(cam));
    }
  }
}

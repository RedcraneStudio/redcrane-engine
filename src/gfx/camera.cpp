/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

#include "idriver.h"
#include "support/unproject.h"
namespace redc
{
  namespace gfx
  {
    Camera::Camera(Camera const& cam) noexcept
    {
      *this = cam;
    }
    Camera& Camera::operator=(Camera const& cam) noexcept
    {
      this->projection_mode = cam.projection_mode;
      switch(cam.projection_mode)
      {
        case Camera_Type::Perspective:
          this->perspective = cam.perspective;
          break;
        case Camera_Type::Orthographic:
          this->ortho = cam.ortho;
      }
      this->definition = cam.definition;
      switch(cam.definition)
      {
        case Camera_Definition::Look_At:
          this->look_at = cam.look_at;
          break;
        case Camera_Definition::Pitch_Yaw_Pos:
          this->fp = cam.fp;
      }

      return *this;
    }
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
    Camera make_isometric_camera(IDriver const& driver) noexcept
    {
      auto cam = Camera{};

      auto win_size = driver.window_extents();

      cam.projection_mode = Camera_Type::Perspective;
      cam.perspective = Perspective_Cam_Params{glm::radians(50.0f),
                                               win_size.x / (float) win_size.y,
                                               .01f, 100.0f};

      cam.definition = Camera_Definition::Pitch_Yaw_Pos;
      cam.fp.pos = glm::vec3(5, 5, 5);
      cam.fp.yaw = glm::radians(-45.0f);
      cam.fp.pitch = glm::radians(45.0f);

      return cam;
    }
    Camera make_fps_camera(IDriver const& driver) noexcept
    {
      auto cam = Camera{};

      auto win_size = driver.window_extents();

      cam.projection_mode = Camera_Type::Perspective;
      cam.perspective = Perspective_Cam_Params{glm::radians(50.0f),
                                               win_size.x / (float) win_size.y,
                                               .01f, 68.0f * M_PI / 180.0f};

      cam.definition = Camera_Definition::Pitch_Yaw_Pos;
      cam.fp.pos = glm::vec3(0.0);
      cam.fp.yaw = 0.0f;
      cam.fp.pitch = 0.0f;
      return cam;
    }
    void use_camera(IDriver& driver, Camera const& cam) noexcept
    {
      driver.active_shader()->set_view(camera_view_matrix(cam));
      driver.active_shader()->set_projection(camera_proj_matrix(cam));
    }

#if 0
    void apply_pan(Camera& cam, Vec<int> np, Vec<int> op, IDriver& dv) noexcept
    {
      auto oworld = gfx::unproject_screen(dv, cam, glm::mat4(1.0f), op);
      auto nworld = gfx::unproject_screen(dv, cam, glm::mat4(1.0f), np);

      cam.fp.pos += oworld - nworld;

      // This will give erroneous results if the user clicks on the part of the
      // screen with nothing drawn on it. It will also produce strange results
      // if the user clicks on any surface with an angle. Basically only use
      // function after drawing the map surface, or some flat surface just for
      // this or something. So TODO: Put some checks in place so we don't go
      // messing with the camera zoom and whatnot.
    }
    void apply_zoom(Camera& c, double delta, Vec<int> mp, IDriver& d) noexcept
    {
      auto mworld = gfx::unproject_screen(d, c, glm::mat4(1.0f), mp);
      apply_zoom(c, delta, mworld);
    }
    void apply_zoom(Camera& cam, double delta, glm::vec3 world) noexcept
    {
      auto ray_to_cam = glm::normalize(world - cam.fp.pos);
      cam.fp.pos += ray_to_cam * (float) delta;
    }
#endif
  }
}

/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <glm/glm.hpp>
namespace game
{
  namespace gfx
  {
    enum class Camera_Type
    {
      Perspective, Orthographic
    };

    struct Perspective_Cam_Params
    {
      float fov, aspect, near, far;
    };

    struct Ortho_Cam_Params
    {
      float left, right, bottom, top, near, far;
    };

    enum class Camera_Definition
    {
      Look_At,
      Pitch_Yaw_Pos
    };

    struct Eye_Look_Up
    {
      glm::vec3 eye;
      glm::vec3 look;
      glm::vec3 up;
    };
    struct Pitch_Yaw_Pos
    {
      glm::vec3 pos;
      float pitch = 0.0;
      float yaw = 0.0;
    };

    struct Camera
    {
      // A camera also includes a perspective and orthographic mode.
      Camera_Type projection_mode;
      union
      {
        Perspective_Cam_Params perspective;
        Ortho_Cam_Params ortho;
      };

      // Camera position, orientation and lookpos.
      Camera_Definition definition;
      union
      {
        Eye_Look_Up look_at;
        Pitch_Yaw_Pos fp; // stands for first-person
      };
    };

    glm::mat4 camera_view_matrix(Camera const& cam) noexcept;
    glm::mat4 camera_proj_matrix(Camera const& cam) noexcept;

    Camera make_isometric_camera() noexcept;
    Camera make_fps_camera() noexcept;

    struct IDriver;
    void use_camera(IDriver& driver, Camera const& cam) noexcept;
  }
}

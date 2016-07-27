/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../common/vec.h"
#include <glm/glm.hpp>

// TODO: Find the one place this is required and figure out someway to make it
// pretty.
#undef near
#undef far
namespace redc
{
  namespace gfx
  {
    enum class Camera_Type
    {
      Perspective, Orthographic
    };

    struct Perspective_Cam_Params
    {
      float fov;
      float aspect;
      float near;
      float far;
    };

    struct Ortho_Cam_Params
    {
      float left;
      float right;
      float bottom;
      float top;
      float near;
      float far;
    };

    enum class Camera_Definition
    {
      Look_At,
      Pitch_Yaw_Pos
    };

    struct Eye_Look_Up
    {
      Eye_Look_Up() noexcept {}
      Eye_Look_Up(Eye_Look_Up const&) = default;
      Eye_Look_Up& operator=(Eye_Look_Up const&) = default;

      glm::vec3 eye;
      glm::vec3 look;
      glm::vec3 up;
    };
    struct Pitch_Yaw_Pos
    {
      Pitch_Yaw_Pos() noexcept {}
      Pitch_Yaw_Pos(Pitch_Yaw_Pos const&) = default;
      Pitch_Yaw_Pos& operator=(Pitch_Yaw_Pos const&) = default;

      glm::vec3 pos;
      float pitch = 0.0;
      float yaw = 0.0;
    };

    struct Camera
    {
      Camera() noexcept {}
      Camera(Camera const& cam) noexcept;
      Camera& operator=(Camera const& cam) noexcept;

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
    glm::mat4 camera_model_matrix(Camera const& cam) noexcept;

    inline glm::vec3 camera_forward(Camera const& cam) noexcept
    {
      return cam.look_at.look - cam.look_at.eye;
    }

    Camera make_isometric_camera(Vec<int> win_size) noexcept;
    Camera make_fps_camera(Vec<int> win_size) noexcept;

    struct IDriver;
    void use_camera(IDriver& driver, Camera const& cam) noexcept;

    /*!
     * \brief Calculates the amount to pan given to mouse coordinates.
     *
     * Costs two glReadPixels for depth. Requires the camera to be in first-
     * person specification mode.
     */
    //void apply_pan(Camera& cam, Vec<int> np, Vec<int> op, IDriver& d) noexcept;

    //void apply_zoom(Camera& c, double delta, Vec<int> mp, IDriver& d) noexcept;
    //void apply_zoom(Camera& c, double delta, glm::vec3 world) noexcept;
  }
}

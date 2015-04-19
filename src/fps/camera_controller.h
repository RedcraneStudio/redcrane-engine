/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../gfx/camera.h"
namespace game
{
  namespace fps
  {
    struct Camera_Controller
    {
      void set_yaw_limit(double limit, bool use = true) noexcept;
      void set_pitch_limit(double limit, bool use = true) noexcept;

      void apply_delta_yaw(double val) noexcept;
      void apply_delta_pitch(double val) noexcept;

      void camera(gfx::Camera& cam) noexcept;
      gfx::Camera* camera() const noexcept;
    private:
      bool limit_yaw_;
      double yaw_limit_;

      bool limit_pitch_;
      double pitch_limit_;

      gfx::Camera* cam_;
    };
  }
}

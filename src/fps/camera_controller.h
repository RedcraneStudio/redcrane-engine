/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../gfx/camera.h"
namespace redc
{
  namespace fps
  {
    struct Camera_Controller
    {
      void set_yaw_limit(double limit, bool use = true);
      void set_pitch_limit(double limit, bool use = true);

      void apply_delta_yaw(gfx::Camera& cam, double val);
      void apply_delta_pitch(gfx::Camera& cam, double val);
    private:
      bool limit_yaw_;
      double yaw_limit_;

      bool limit_pitch_;
      double pitch_limit_;
    };
  }
}

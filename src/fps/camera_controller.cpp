/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "camera_controller.h"
#include "../common/clamp.hpp"
namespace redc
{
  namespace fps
  {
    void Camera_Controller::set_yaw_limit(double limit, bool use)
    {
      yaw_limit_ = limit;
      limit_yaw_ = use;
    }
    void Camera_Controller::set_pitch_limit(double limit, bool use)
    {
      pitch_limit_ = limit;
      limit_pitch_ = use;
    }
    void Camera_Controller::apply_delta_yaw(gfx::Camera& cam, double val)
    {
      if(limit_yaw_)
      {
        cam.fp.yaw = (float) clamp(-yaw_limit_, yaw_limit_, cam.fp.yaw + val);
      }
      else cam.fp.yaw += val;
    }
    void Camera_Controller::apply_delta_pitch(gfx::Camera& cam, double val)
    {
      if(limit_pitch_)
      {
        cam.fp.pitch = (float) clamp(-pitch_limit_, pitch_limit_,
                                     cam.fp.pitch + val);
      }
      else cam.fp.pitch += val;
    }
  }
}

/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "camera_controller.h"
namespace redc
{
  namespace fps
  {
    namespace
    {
      template <class T>
      T clamp(T lower_bound, T upper_bound, T val) noexcept
      {
        if(val < lower_bound) { return lower_bound; }
        else if(upper_bound < val) { return upper_bound; }
        else { return val; }
      }
    }

    void Camera_Controller::set_yaw_limit(double limit, bool use) noexcept
    {
      yaw_limit_ = limit;
      limit_yaw_ = use;
    }
    void Camera_Controller::set_pitch_limit(double limit, bool use) noexcept
    {
      pitch_limit_ = limit;
      limit_pitch_ = use;
    }
    void Camera_Controller::apply_delta_yaw(double val) noexcept
    {
      if(!cam_) return;
      if(limit_yaw_)
      {
        cam_->fp.yaw = clamp(-yaw_limit_, yaw_limit_, cam_->fp.yaw + val);
      }
      else cam_->fp.yaw += val;
    }
    void Camera_Controller::apply_delta_pitch(double val) noexcept
    {
      if(!cam_) return;
      if(limit_pitch_)
      {
        cam_->fp.pitch = clamp(-pitch_limit_, pitch_limit_,
                               cam_->fp.pitch + val);
      }
      else cam_->fp.pitch += val;
    }

    void Camera_Controller::camera(gfx::Camera& cam) noexcept
    {
      cam_ = &cam;
    }
    gfx::Camera* Camera_Controller::camera() const noexcept
    {
      return cam_;
    }
  }
}

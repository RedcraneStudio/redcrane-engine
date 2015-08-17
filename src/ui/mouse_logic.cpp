/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "mouse_logic.h"

#include "../gfx/idriver.h"
#include "../gfx/camera.h"
#include "../gfx/support/unproject.h"

#include <glm/gtc/matrix_transform.hpp>

namespace game { namespace ui
{
  void On_Click_Handler::operator()(Mouse_State const& ms) noexcept
  {
    if(ms.buttons & button_)
    {
      cb_(ms);
    }
  }
  void On_Release_Handler::operator()(Mouse_State const& ms) noexcept
  {
    if(clicked_ && !(ms.buttons & button_))
    {
      cb_(ms);
      clicked_ = false;
    }
    else if(ms.buttons & button_)
    {
      clicked_ = true;
    }
  }

  void Camera_Rotation_Helper::step_mouse(Mouse_State const& ms) noexcept
  {
    // Once the player clicks the button, record their current mouse position.
    if(ms.buttons & btn_ && !clicked_)
    {
      clicked_ = true;
      prev_pos_ = ms.position;

      // Mark their map position as well, this way we don't have to recalculate
      // it a bunch later.
      auto world_pos = gfx::unproject_screen(driver_, camera_, glm::mat4(1.0f),
                                             ms.position);
      map_pos_ = {world_pos.x, world_pos.z};

      // TODO: Capture mouse here.
    }
    // Simply an else won't work because of the !clicked_ condition above.
    if(!(ms.buttons & btn_))
    {
      clicked_ = false;

      // TODO: Uncapture mouse here.
    }

    // Check to see if the user moved their mouse while it was clicked down.
    if(clicked_ && ms.position != prev_pos_)
    {
      // Figure out the mouse movement
      auto mouse_dif = ms.position.x - prev_pos_.x;

      // Figure out how much we rotate. We can't just use the length because
      // we need to know direction as well, plus we only care about x axis
      // movement.
      float angle = mouse_dif * speed_;

      // TODO: Gah, fix this already!
      auto map_pos_vec3 = glm::vec3(map_pos_.x, 0.0f, map_pos_.y);
      // Generate a matrix to:
      // - Move the camera so its origin is the map pos
      // - Rotate it by the angle
      // - Move it back.
      auto mat = glm::mat4(1.0f);
      mat = glm::translate(mat, map_pos_vec3);
      mat = glm::rotate(mat, angle, glm::vec3(0.0f, 1.0f, 0.0f));
      mat = glm::translate(mat, -map_pos_vec3);

      // Apply the matrix
      auto cam_pos_vec4 = glm::vec4(camera_.fp.pos, 1.0f);
      camera_.fp.pos = static_cast<glm::vec3>(mat * cam_pos_vec4);

      // Change the camera yaw by an opposite amount.
      camera_.fp.yaw += -angle;

      prev_pos_ = ms.position;
    }
  }
  void Camera_Orientation_Helper::step_mouse(Mouse_State const& ms) noexcept
  {
    if(ms.buttons & btn_ && cur_state_ == Cur_State::Start)
    {
      cur_state_ = Cur_State::Clicked;
    }
    else if(~ms.buttons & btn_ && cur_state_ == Cur_State::Clicked)
    {
      cur_state_ = Cur_State::Released;
    }
    else if(ms.buttons & btn_ && cur_state_ == Cur_State::Released)
    {
      cur_state_ = Cur_State::Almost_Done;
    }
    else if(~ms.buttons & btn_ && cur_state_ == Cur_State::Almost_Done)
    {
      cur_state_ = Cur_State::Done;
    }

    if(cur_state_ == Cur_State::Released || cur_state_ == Cur_State::Clicked)
    {
      auto dif = ms.position - old_pos_;

      camera_.fp.pitch += dif.y * y_speed_;
      camera_.fp.yaw += dif.x * x_speed_;
    }

    old_pos_ = ms.position;
  }
} }

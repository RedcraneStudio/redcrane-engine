/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "camera.h"

#include <glm/gtx/rotate_vector.hpp>
namespace redc
{
  // dif is expected to be scaled according to some setting, that is, we don't
  // want to deal with it here!
  void reposition_camera(gfx::Camera& cam, Vec<float> dif) noexcept
  {
    glm::vec3 eye_dir = cam.look_at.look - cam.look_at.eye;

    auto cross_eye = glm::cross(glm::normalize(eye_dir), cam.look_at.up);

    // Rotate the eye direction by a given amount by dif.
    eye_dir = glm::rotate(eye_dir, dif.x, cam.look_at.up);
    eye_dir = glm::rotate(eye_dir, dif.y, cross_eye);
    cam.look_at.look = cam.look_at.eye + eye_dir;
  }
}

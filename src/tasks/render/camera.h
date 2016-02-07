/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../../gfx/camera.h"
namespace redc
{
  void reposition_camera(gfx::Camera& cam, Vec<float> dif) noexcept;
}

/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include "mesh.h"
namespace game
{
  void write_obj(std::string fn, Mesh_Data const& mesh) noexcept;
}

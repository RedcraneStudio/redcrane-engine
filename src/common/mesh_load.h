/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include "mesh.h"
namespace game
{
  // The reason this function accepts a mesh impl and doesn't just return mesh
  // data is because error handling is as easy as not preparing the mesh, then
  // again it will go unnoticed. That being said an empty mesh could do fine.
  void load_obj(std::string obj, Mesh& m) noexcept;
}

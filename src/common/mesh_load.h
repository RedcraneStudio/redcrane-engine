/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include <istream>
#include "mesh.h"
namespace game
{
  Vert_Ref parse_vert_ref(std::string str) noexcept;
  glm::vec3 parse_vec3(std::istream& stream) noexcept;

  // The reason this function accepts a mesh impl and doesn't just return mesh
  // data is because error handling is as easy as not preparing the mesh, then
  // again it will go unnoticed. That being said an empty mesh could do fine.
  void load_obj(std::string obj, Mesh& m) noexcept;
  void load_obj(std::istream& obj, Mesh& m) noexcept;
}

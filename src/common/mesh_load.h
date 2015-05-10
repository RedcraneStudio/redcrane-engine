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
  // Describes the index into a bunch of different buffers.
  struct Vertex_Indices
  {
    unsigned int vertex;
    unsigned int normal;
    unsigned int tex_coord;

    unsigned int index;
  };
  bool operator<(Vertex_Indices const& v1, Vertex_Indices const& v2) noexcept;
  bool operator==(Vertex_Indices const& v1, Vertex_Indices const& v2) noexcept;
  Vertex_Indices parse_vertex_indices(std::string str) noexcept;
  glm::vec3 parse_vec3(std::istream& stream) noexcept;

  // The reason this function accepts a mesh impl and doesn't just return mesh
  // data is because error handling is as easy as not preparing the mesh, then
  // again it will go unnoticed. That being said an empty mesh could do fine.
  void load_obj(std::string obj, Mesh& m) noexcept;
  void load_obj(std::istream& obj, Mesh& m) noexcept;
}

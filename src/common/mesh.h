/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <glm/glm.hpp>
#include <istream>
#include <string>
#include <vector>
#include "aabb.h"
namespace strat
{
  struct Face
  {
    unsigned int vertex;
    unsigned int normal;
    unsigned int tex_coord;
  };
  bool operator<(Face const& f1, Face const& f2) noexcept;
  bool operator==(Face const& f1, Face const& f2) noexcept;
  Face parse_face(std::string str) noexcept;
  glm::vec3 parse_vec3(std::istream& stream) noexcept;

  struct Mesh
  {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> tex_coords;
    std::vector<unsigned int> faces;

    static Mesh from_contents(std::string str) noexcept;
    static Mesh from_stream(std::istream&& s) noexcept;
    static Mesh from_file(std::string f) noexcept;

    Mesh() noexcept = default;
  };

  AABB generate_aabb(Mesh const& mesh) noexcept;
}

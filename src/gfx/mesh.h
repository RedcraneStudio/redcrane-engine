/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <glm/glm.hpp>
#include <istream>
#include <string>
#include <vector>
namespace survive
{
  struct Mesh
  {
    std::vector<glm::vec3> vertices;
    std::vector<glm::ivec3> faces;

    static Mesh from_stream(std::istream&& s) noexcept;
    static Mesh from_file(std::string f) noexcept;

  private:
    Mesh() noexcept = default;
  };
}

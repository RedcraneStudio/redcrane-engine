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
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> tex_coords;
    std::vector<unsigned int> faces;

    static Mesh from_contents(std::string str) noexcept;
    static Mesh from_stream(std::istream&& s) noexcept;
    static Mesh from_file(std::string f) noexcept;

  private:
    Mesh() noexcept = default;
  };
}

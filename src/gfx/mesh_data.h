/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 *
 * This file defines different representations for mesh data along with
 * functions to convert between the representations.
 */
#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <boost/optional.hpp>
namespace game
{
  struct Vertex
  {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
  };

  std::tuple<std::vector<glm::vec3>,
             std::vector<glm::vec3>,
             std::vector<glm::vec2> >
    separate_vertices(std::vector<Vertex> const& v) noexcept;

  template <class T> std::vector<T>
  get_component_vector(std::vector<Vertex> const& v, T Vertex::* c) noexcept
  {
    using std::begin; using std::end;

    auto ret = std::vector<T>{};
    ret.resize(v.size());

    std::transform(begin(v), end(v), begin(ret),
    [&](auto const& vertex)
    {
      return vertex.*c;
    });
    return ret;
  }

  struct Vert_Ref
  {
    boost::optional<unsigned int> position;
    boost::optional<unsigned int> normal;
    boost::optional<unsigned int> tex_coord;
  };

  bool operator==(Vert_Ref const& v1, Vert_Ref const& v2) noexcept;
  bool operator<(Vert_Ref const& lhs, Vert_Ref const& rhs) noexcept;

  // TODO: Write some easy move-enabled conversion code between these.

  struct Indexed_Mesh_Data
  {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> elements;
  };

  struct Ordered_Mesh_Data
  {
    std::vector<Vertex> vertices;
  };

  struct Indexed_Split_Mesh_Data
  {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> tex_coords;

    std::vector<Vert_Ref> indices;
  };

  struct Ordered_Split_Mesh_Data
  {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> tex_coords;
  };
}

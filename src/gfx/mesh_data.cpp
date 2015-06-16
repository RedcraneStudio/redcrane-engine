/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "mesh_data.h"
namespace game
{
  bool operator==(Vert_Ref const& lhs, Vert_Ref const& rhs) noexcept
  {
    return lhs.position == rhs.position &&
           lhs.normal == rhs.normal &&
           lhs.tex_coord == rhs.tex_coord;
  }
  bool operator<(Vert_Ref const& lhs, Vert_Ref const& rhs) noexcept
  {
    if(lhs.position < rhs.position) return true;
    else if(lhs.position == rhs.position)
    {
      if(lhs.normal < rhs.normal) return true;
      else if(lhs.normal == rhs.normal)
      {
        if(lhs.tex_coord < rhs.tex_coord) return true;
        else return false;
      }
    }
    return false;
  }

  std::tuple<std::vector<glm::vec3>,
             std::vector<glm::vec3>,
             std::vector<glm::vec2> >
  separate_vertices(std::vector<Vertex> const& v) noexcept
  {
    std::tuple<std::vector<glm::vec3>,
               std::vector<glm::vec3>,
               std::vector<glm::vec2> > ret;

    std::get<0>(ret) = get_component_vector(v, &Vertex::position);
    std::get<1>(ret) = get_component_vector(v, &Vertex::normal);
    std::get<2>(ret) = get_component_vector(v, &Vertex::uv);

    return ret;
  }
}

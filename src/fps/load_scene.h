/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <glm/glm.hpp>

#include "../common/json.h"
#include "../gfx/mesh_chunk.h"
#include "../collisionlib/triangle.h"
namespace game { namespace fps
{
  struct Object
  {
    gfx::Mesh_Chunk mesh;
    glm::mat4 model;
  };

  struct Scene
  {
    glm::vec3 player_pos;

    std::vector<collis::Triangle> collision_triangles;

    // Give just one mesh chunk ownership of the mesh, the rest of the chunks
    // in the vector can point to it since as long as everything is destructed
    // at the same time it shouldn't be a problem.
    std::vector<Object> objects;
  };

  Scene load_scene(std::string fn, std::unique_ptr<Mesh> msh) noexcept;
} }

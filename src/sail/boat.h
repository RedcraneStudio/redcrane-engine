/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include <glm/glm.hpp>
#include "../gfx/mesh_chunk.h"
#include "../use/mesh_cache.h"
namespace redc
{
  struct Hull_Attachment_Def
  {
    // All points in boat model space. They basically define the translation
    // from attachment space to boat space.
    glm::vec3 sail;
    glm::vec3 rudder;
    glm::vec3 gun;
  };
  struct Hull_Desc
  {
    std::string name;
    gfx::Mesh_Chunk mesh;
    Hull_Attachment_Def attachments;
  };

  namespace gfx { struct Mesh_Cache; }
  Hull_Desc build_hull_desc(gfx::Mesh_Cache& mc, std::string name,
                            std::string model_name,
                            Hull_Attachment_Def attachments) noexcept;

  template <int N>
  struct Object_Desc
  {
    std::string name;
    gfx::Mesh_Chunk mesh;
  };

  // Make these all not the same type
  using Sail_Desc =   Object_Desc<0>;
  using Rudder_Desc = Object_Desc<1>;
  using Gun_Desc =    Object_Desc<2>;

  Sail_Desc build_sail_desc(gfx::Mesh_Cache& mc, std::string name,
                            std::string model_name) noexcept;
  Rudder_Desc build_rudder_desc(gfx::Mesh_Cache& mc, std::string name,
                                std::string model_name) noexcept;
  Gun_Desc build_gun_desc(gfx::Mesh_Cache& mc, std::string name,
                          std::string model_name) noexcept;

  struct Boat_Config
  {
    Hull_Desc* hull;
    Sail_Desc* sail;
    Rudder_Desc* rudder;
    Gun_Desc* gun;
  };

  struct Boat_Render_Config
  {
    Hull_Attachment_Def attachments;

    gfx::Mesh_Chunk hull;
    gfx::Mesh_Chunk rudder;
    gfx::Mesh_Chunk sail;
    gfx::Mesh_Chunk gun;

    glm::mat4 model;
  };

  Boat_Render_Config build_boat_render_config(Boat_Config&) noexcept;

  struct Boat_Descs
  {
    std::vector<Hull_Desc> hull_descs;
    std::vector<Sail_Desc> sail_descs;
    std::vector<Rudder_Desc> rudder_descs;
    std::vector<Gun_Desc> gun_descs;
  };

  // Defined in boat_def.cpp
  Boat_Descs build_default_descs(gfx::Mesh_Cache& mc) noexcept;
  void log_boat_descs(Boat_Descs&) noexcept;
}

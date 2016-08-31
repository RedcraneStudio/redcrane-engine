/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include <memory>

#include "common/observer_ptr.hpp"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

#include "gfx/scene.h"

#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
namespace redc
{
  struct Rendering_Component
  {
    virtual ~Rendering_Component() {}

    gfx::Asset asset;
  };

  struct Map;
  struct Physics_Component
  {
    virtual ~Physics_Component() {}

    observer_ptr<Map> map;

    std::unique_ptr<btRigidBody> body;
  };

  struct Spawns_Decl
  {
    enum
    {
      Random,
    } choice;
    std::vector<glm::vec3> positions;
  };

  enum class Shape_Type
  {
    Sphere
  };

  struct Sphere
  {
    float radius;
  };

  struct Shape
  {
    Shape_Type type;

    union
    {
      Sphere sphere;
    };
  };

  struct Physics_Event_Decl
  {
    std::string event_name;
    glm::vec3 position;
    Shape shape;
  };

  struct Physics_Decl
  {
    glm::vec3 gravity;
  };

  struct Map
  {
    std::string name;

    tinygltf::Scene scene;
    short players;

    Spawns_Decl spawns;

    // When we are dealing with a glTF, these are names of accessors.
    std::string collision_vertices_source;
    std::string collision_indices_source;

    Physics_Decl physics;
    std::vector<Physics_Event_Decl> physics_events;

    // Initialized later, if necessary.
    std::unique_ptr<Rendering_Component> render;
    std::unique_ptr<Physics_Component> collision;

  };

  bool load_spawns(Spawns_Decl& spawns, rapidjson::Value const& val,
                   std::string* err);
  bool load_map_json(Map& map, rapidjson::Value const& doc, std::string* err);

}

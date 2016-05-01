/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include "common/observer_ptr.hpp"
#include "gfx/mesh_chunk.h"
#include "gfx/mesh_data.h"
namespace redc
{
  struct Rendering_Component
  {
    gfx::Mesh_Chunk chunk;
  };

  // This physics component should really be more general, but for now it will
  // work fine. Best later to put btRigidBody in the base component and the
  // shape in the derived class.
  struct Server;
  struct Map;
  struct Physics_Component
  {
    Physics_Component(Server& server) : server_(&server) {}
    ~Physics_Component();

    observer_ptr<Map> map;

    std::unique_ptr<btTriangleIndexVertexArray> vertices;
    std::unique_ptr<btBvhTriangleMeshShape> shape;
    std::unique_ptr<btRigidBody> body;

  private:
    Server* server_;
  };

  struct Map
  {
    // Construct the mesh
    Map() : mesh(std::make_unique<Indexed_Mesh_Data>()) {}
    Map(Indexed_Mesh_Data&& data)
      : mesh(std::make_unique<Indexed_Mesh_Data>(std::move(data))) {}

    // This needs to be constructed on the heap so we can reference it by
    // pointer elsewhere (notably the btTriangleIndexVertexArray).
    // Also, as a side effect we get an implicit move constructor and no copy
    // constructor.
    std::unique_ptr<Indexed_Mesh_Data> mesh;

    Maybe_Owned<Rendering_Component> render;
    Maybe_Owned<Physics_Component> physics;
  };

}

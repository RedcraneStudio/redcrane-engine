/*
 * Copyright (c) 2016 Luke San Antonio
 * All rights reserved.
 *
 * This file provides the implementation for the engine's C interface,
 * specifically tailored for LuaJIT's FFI facilities.
 */

#include "redcrane.hpp"

#include "../gfx/support/format.h"
#include "../gfx/support/allocate.h"
#include "../gfx/support/write_data_to_mesh.h"

namespace redc
{
  struct Model_Visitor : boost::static_visitor<glm::mat4>
  {
    glm::mat4 operator()(Mesh_Object const& msh) const
    {
      return msh.model;
    }
    glm::mat4 operator()(Cam_Object const& cam) const
    {
      return glm::inverse(gfx::camera_view_matrix(cam.cam));
    }
  };

  Upload_Map_Mesh::ptr_type Upload_Map_Mesh::operator()(ptr_type, Map* map)
  {
    auto mesh_repr = this->driver->make_mesh_repr();
    gfx::allocate_standard_mesh_buffers(map->render_mesh.vertices.size(),
                                       map->render_mesh.elements.size(),
                                       *mesh_repr,
                                       Usage_Hint::Draw,
                                       Upload_Hint::Static);
    gfx::format_standard_mesh_buffers(*mesh_repr);

    auto mesh_chunk = std::make_unique<gfx::Mesh_Chunk>();

    *mesh_chunk = gfx::write_data_to_mesh(map->render_mesh,
                                     std::move(mesh_repr), 0, 0);

    return mesh_chunk;
  }

  glm::mat4 object_model(Object const& obj)
  {
    // Find current
    auto this_model = boost::apply_visitor(Model_Visitor{}, obj.obj);

    // Find parent
    glm::mat4 parent_model(1.0f);
    if(obj.parent)
    {
      parent_model = object_model(*obj.parent);
    }

    // First apply the child transformations then the parent transformations.
    return parent_model * this_model;
  }

  Map::Map(Indexed_Mesh_Data&& data) noexcept
  : render_mesh(std::move(data)), collis_mesh(),
    collis_shape(&collis_mesh, true, false)
  {
    btIndexedMesh indexed_mesh;
    indexed_mesh.m_numTriangles = render_mesh.elements.size() / 3;
    indexed_mesh.m_triangleIndexBase = (unsigned char*) &render_mesh.elements[0];
    indexed_mesh.m_triangleIndexStride = 3 * sizeof(unsigned int);
    indexed_mesh.m_numVertices = render_mesh.elements.size();
    indexed_mesh.m_vertexBase = (unsigned char*) &render_mesh.vertices[0];
    indexed_mesh.m_vertexStride = sizeof(Vertex);
    indexed_mesh.m_vertexType = PHY_FLOAT;

    collis_mesh.addIndexedMesh(indexed_mesh, PHY_INTEGER);
    collis_shape.buildOptimizedBvh();
  }

  Server::Server()
  {
    bt_config = std::make_unique<btDefaultCollisionConfiguration>();
    bt_dispatcher = std::make_unique<btCollisionDispatcher>(bt_config.get());
    bt_broadphase = std::make_unique<btDbvtBroadphase>();
    bt_solver = std::make_unique<btSequentialImpulseConstraintSolver>();

    bt_world = std::make_unique<btDiscreteDynamicsWorld>(bt_dispatcher.get(),
                                                         bt_broadphase.get(),
                                                         bt_solver.get(),
                                                         bt_config.get());

    bt_world->setGravity(btVector3(0.0f, -9.81f, 0.0f));
  }

  void Server::req_player()
  {
    // New id
    auto id = this->index_gen.get();

    // Be careful about reusing the memory for the player controller here.
    at_id(this->players, id).controller.reset();

    // New player, notify anyone who cares
    push_outgoing_event(New_Player_Event{id, true});
  }
  Player& Server::player(player_id id)
  {
    return players[id-1];
  }
}

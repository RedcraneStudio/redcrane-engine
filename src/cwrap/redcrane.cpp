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

  void Engine::push_outgoing_event(Event event)
  {
    event_queue_.push_outgoing_event(std::move(event));
  }
  bool Engine::poll_event(Event& event)
  {
    return event_queue_.poll_event(event);
  }

  struct Client_Event_Visitor : boost::static_visitor<>
  {
    Client_Event_Visitor(Client& client) : client_(&client) {}

    void operator()(New_Player_Event const& event) const
    {
    }
    void operator()(Map_Loaded_Event const& event) const
    {
      REDC_ASSERT_MSG(event.map != nullptr, "Map not provided to client");

      auto& map = event.map;

      // Load the map into a mesh chunk
      auto mesh_repr = client_->driver->make_mesh_repr();
      gfx::allocate_standard_mesh_buffers(map->mesh->vertices.size(),
                                          map->mesh->elements.size(),
                                          *mesh_repr,
                                          Usage_Hint::Draw,
                                          Upload_Hint::Static);
      gfx::format_standard_mesh_buffers(*mesh_repr);

      // Make a rendering component
      auto rendering_component = std::make_unique<Rendering_Component>();

      // Set the chunk
      rendering_component->chunk = gfx::write_data_to_mesh(
          *map->mesh, std::move(mesh_repr), 0, 0);

      // Give the rendering component to the map
      map->render = std::move(rendering_component);
    }
  private:
    Client* client_;
  };

  void Client::process_event(event_t const& event)
  {
    boost::apply_visitor(Client_Event_Visitor{*this}, event);

    for(auto const& scene : scenes)
    {
      if(scene)
      {
        scene->process_event(event);
      }
    }
  }

  struct Server_Event_Visitor : boost::static_visitor<>
  {
    Server_Event_Visitor(Server& server) : server_(&server) {}

    void operator()(New_Player_Event const& event) const {}
    void operator()(Map_Loaded_Event const& event) const;
  private:
    Server* server_;
  };
  void Server_Event_Visitor::operator()(Map_Loaded_Event const& event) const
  {
    REDC_ASSERT_MSG(event.map != nullptr, "New map not provided to server");

    auto& map = event.map;

    // By passing the physics component the server we can be sure when the
    // map is destructed it is removed from the world as a collision object.
    // TODO: This construct / destruct and remove of the rigid body should
    // either be more localized right here, or abstracted in a different way.
    // See map.h Physics_Component note about that.
    auto physics = make_maybe_owned<Physics_Component>(*server_);

    btIndexedMesh indexed_mesh;
    indexed_mesh.m_numTriangles = map->mesh->elements.size() / 3;
    indexed_mesh.m_triangleIndexBase = (unsigned char*) &map->mesh->elements[0];
    indexed_mesh.m_triangleIndexStride = 3 * sizeof(unsigned int);
    indexed_mesh.m_numVertices = map->mesh->elements.size();
    indexed_mesh.m_vertexBase = (unsigned char*) &map->mesh->vertices[0];
    indexed_mesh.m_vertexStride = sizeof(Vertex);
    indexed_mesh.m_vertexType = PHY_FLOAT;

    physics->vertices = std::make_unique<btTriangleIndexVertexArray>();
    physics->vertices->addIndexedMesh(indexed_mesh, PHY_INTEGER);

    physics->shape = std::make_unique<btBvhTriangleMeshShape>(
        physics->vertices.get(), false, true);

    btRigidBody::btRigidBodyConstructionInfo map_rb_info
            {0, nullptr, physics->shape.get()};

    physics->body = std::make_unique<btRigidBody>(map_rb_info);

    server_->bt_world->addRigidBody(physics->body.get());

    event.map->physics = std::move(physics);
  }
  void Server::process_event(event_t const& event)
  {
    boost::apply_visitor(Server_Event_Visitor{*this}, event);
  }

  Server::Server(Engine& eng) : engine_(&eng)
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
    engine_->push_outgoing_event(New_Player_Event{id, true});
  }
  Player& Server::player(player_id id)
  {
    return players[id-1];
  }

  void Server::load_map(std::string const& filename)
  {
    log_i("Loading map: '%'", filename);

    // Right now this is a filename that points to a model
    auto mesh_data = engine_->mesh_cache->load(filename);

    // Construct the map
    auto map = std::make_unique<Map>(std::move(mesh_data));
    auto map_observer = observer_ptr<Map>{map.get()};

    // Keep the map around
    maps.push_back(std::move(map));

    // Queue the new map event. This should inform the client and a Server
    // routine that the map has been loaded. The client will probably upload
    // the data to GPU memory and the server will show bullet the data so we
    // have collision.
    engine_->push_outgoing_event(Map_Loaded_Event{map_observer});

  }

  struct Scene_Event_Visitor : public boost::static_visitor<>
  {
    Scene_Event_Visitor(Scene& scene) : scene_(&scene) {}

    void operator()(New_Player_Event const& event) const;
    void operator()(Map_Loaded_Event const& event) const;
  private:
    Scene* scene_;
  };

  void Scene_Event_Visitor::operator()(New_Player_Event const& np) const
  {
    if(np.owned)
    {
      // This is our new owned player.
      // TODO: Verify the address will never change!
      // Nevermind it won't but it's still important to keep in mind.

      // TODO: Eventually this can't be a reference because networking (I
      // assume). Store an id here, I don't know what I'm going to do in
      // the controller, etc.
      scene_->active_player = &scene_->engine->server->player(np.id);

      // Give it access to the current input state.
      scene_->active_player->controller.set_input_ref(&scene_->cur_input);

      // Add the controller to the physics world.
      scene_->engine->server->bt_world->addAction(
          &scene_->active_player->controller);
    }
  }
  void Scene_Event_Visitor::operator()(Map_Loaded_Event const& event) const
  {
    scene_->active_map = event.map;
    scene_->lightmap = load_texture(*scene_->engine->client->driver,
                                    "../assets/tex/Floor_Light.png");
  }

  void Scene::process_event(event_t const& event)
  {
    boost::apply_visitor(Scene_Event_Visitor{*this}, event);
  }
}

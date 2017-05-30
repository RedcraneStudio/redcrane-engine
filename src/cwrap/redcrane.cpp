/*
 * Copyright (c) 2016 Luke San Antonio
 * All rights reserved.
 *
 * This file provides the implementation for the engine's C interface,
 * specifically tailored for LuaJIT's FFI facilities.
 */

#include "redcrane.hpp"

#include "../gfx/extra/format.h"
#include "../gfx/extra/allocate.h"
#include "../gfx/extra/write_data_to_mesh.h"

#include "../common/json.h"

#include "../assets/minigltf.h"

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

    void operator()(New_Player_Event const&) const
    {
    }
    void operator()(Map_Loaded_Event const& event) const
    {
      REDC_ASSERT_MSG(event.map != nullptr, "Invalid map provided to client");

      // Compile the map glTF scene into a renderable asset
      event.map->render = std::make_unique<Rendering_Component>();

      // Load the cel techniques first
      tinygltf::Scene cel_scene;
      bool gltf_load_succeeded = load_gltf_file(cel_scene,
                                                "../assets/gltf/library-pre.gltf");
      REDC_ASSERT_MSG(gltf_load_succeeded,
        "glTF techniques could not be found; broken installation");
      event.map->render->asset = load_asset(*client_->driver, cel_scene);

      // Then the map
      append_to_asset(*client_->driver, event.map->render->asset,
                      event.map->scene);
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

    void operator()(New_Player_Event const&) const {}
    void operator()(Map_Loaded_Event const& event) const;
  private:
    Server* server_;
  };

  struct Map_Collision : public Physics_Component
  {
    // This is a map-specific shape, so it goes here. Use it to construct the
    // rigid body found in our base class.
    std::unique_ptr<btTriangleIndexVertexArray> vertices;
    std::unique_ptr<btBvhTriangleMeshShape> shape;

    Map_Collision(Server& server) : server_(&server) {}
    ~Map_Collision();

    std::vector<uint8_t>* vertex_data;
    std::vector<uint8_t>* index_data;

    std::vector<std::unique_ptr<btCollisionShape> > click_shapes;
    std::vector<std::unique_ptr<btRigidBody> > click_objects;
  private:
    Server* server_;
  };

  Map_Collision::~Map_Collision()
  {
    // Remove the map body from the world when we are destructed.
    server_->bt_world->removeRigidBody(this->body.get());

    // Remove the other physics click objects from the world
    for(auto body_iter = this->click_objects.begin();
        body_iter != this->click_objects.end(); ++body_iter)
    {
      server_->bt_world->removeRigidBody(body_iter->get());
    }

    delete vertex_data;
    delete index_data;
  }

  void Server_Event_Visitor::operator()(Map_Loaded_Event const& event) const
  {
    REDC_ASSERT_MSG(event.map != nullptr, "Invalid map provided to server");

    auto& map = event.map;

    // Use the accessors from the glTF scene.
    std::vector<uint8_t>* verts_data = new std::vector<uint8_t>();
    tinygltf::Accessor verts_access;

    std::vector<uint8_t>* indices_data = new std::vector<uint8_t>();
    tinygltf::Accessor indices_access;

    if(!resolve_gltf_accessor_data(map->scene, map->collision_vertices_source,
                                   *verts_data, verts_access) ||
       !resolve_gltf_accessor_data(map->scene, map->collision_indices_source,
                                   *indices_data, indices_access))
    {
      log_w("Failed to load collision mesh - map collision will be turned off");
      return;
    }

    // Some assertions (but they shouldn't crash the program)
    if(verts_access.type != TINYGLTF_TYPE_VEC3 &&
       verts_access.type != TINYGLTF_TYPE_VEC4)
    {
      log_w("Collision vertex data must be given as VEC3s or VEC4s");
      return;
    }
    if(indices_access.type != TINYGLTF_TYPE_SCALAR)
    {
      log_w("Collision index data must be given as SCALARs");
      return;
    }

    // Sensible default of the size of a short.
    std::size_t index_size = 2;

    // I hope the bullet knows what we mean when it comes to signedness.
    PHY_ScalarType index_type = PHY_INTEGER;

    // Find the size of each index
    switch(indices_access.componentType)
    {
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
    case TINYGLTF_COMPONENT_TYPE_BYTE:
      index_size = 1;
      index_type = PHY_UCHAR;
      break;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
    case TINYGLTF_COMPONENT_TYPE_SHORT:
      index_size = 2;
      index_type = PHY_SHORT;
      break;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
    case TINYGLTF_COMPONENT_TYPE_INT:
      index_size = 4;
      index_type = PHY_INTEGER;
      break;
    default:
      log_w("Invalid data type of collision index data");
      return;
    }

    std::size_t vert_size = sizeof(float);
    PHY_ScalarType vert_type = PHY_FLOAT;
    switch(verts_access.componentType)
    {
    case TINYGLTF_COMPONENT_TYPE_FLOAT:
      vert_size = sizeof(float);
      vert_type = PHY_FLOAT;
      break;
    case TINYGLTF_COMPONENT_TYPE_DOUBLE:
      vert_size = sizeof(double);
      vert_type = PHY_DOUBLE;
      break;
    }

    auto collision = std::make_unique<Map_Collision>(*server_);

    collision->vertex_data = verts_data;
    collision->index_data = indices_data;

    btIndexedMesh indexed_mesh;

    indexed_mesh.m_numVertices = indices_access.count;
    indexed_mesh.m_triangleIndexBase = &(*indices_data)[indices_access.byteOffset];
    indexed_mesh.m_triangleIndexStride = index_size * 3;

    indexed_mesh.m_numTriangles = indices_access.count / 3;
    indexed_mesh.m_vertexBase = &(*verts_data)[verts_access.byteOffset];
    indexed_mesh.m_vertexStride = vert_size * 3;
    indexed_mesh.m_vertexType = vert_type;

    collision->vertices = std::make_unique<btTriangleIndexVertexArray>();
    collision->vertices->addIndexedMesh(indexed_mesh, index_type);

    collision->shape = std::make_unique<btBvhTriangleMeshShape>(
        collision->vertices.get(), false, true);

    btRigidBody::btRigidBodyConstructionInfo map_rb_info
            {0, nullptr, collision->shape.get()};

    collision->body = std::make_unique<btRigidBody>(map_rb_info);

    server_->bt_world->addRigidBody(collision->body.get());

    // Add physics event shapes
    for(auto iter = map->physics_events.begin();
        iter != map->physics_events.end(); ++iter)
    {
      std::unique_ptr<btCollisionShape> bt_shape;
      Shape const& shape = iter->shape;
      switch(shape.type)
      {
      case Shape_Type::Sphere:
        bt_shape = std::make_unique<btSphereShape>(shape.sphere.radius);
        break;
      default:
        REDC_UNREACHABLE_MSG("Unknown physics event shape type");
      }
      collision->click_shapes.push_back(std::move(bt_shape));
    }
    // Now build rigid bodies for each shape.
    for(std::size_t i = 0; i < map->physics_events.size(); ++i)
    {
      Physics_Event_Decl const& event = map->physics_events[i];

      // Use the index to find the corrosponding shape, this could probably be
      // improved for cases where the same shape is used in multiple "events."
      btRigidBody::btRigidBodyConstructionInfo info {
        0.0f, nullptr, collision->click_shapes[i].get()
      };

      // Don't forget world transform, since we aren't using a motion state.
      btTransform transform;
      transform.setIdentity();
      transform.setOrigin(btVector3(event.position.x, event.position.y,
                                    event.position.z));
      info.m_startWorldTransform = transform;

      std::unique_ptr<btRigidBody> body = std::make_unique<btRigidBody>(info);

      // Link the body to the event itself. This allows us to signal the right
      // event when the time comes.
      body->setUserPointer(&map->physics_events[i]);
      body->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

      // Add the (static) rigid body.
      server_->bt_world->addRigidBody(body.get());
      // Add the body to this vector so the memory isn't deallocated.
      collision->click_objects.push_back(std::move(body));
    }

    map->physics = std::move(collision);

    // The map was successfully loaded, notify Lua
    Lua_Event map_loaded_lua_event;
    map_loaded_lua_event.type = Lua_Event::Map_Loaded;
    map_loaded_lua_event.map = map.get();
    server_->push_lua_event(map_loaded_lua_event);

  }
  void Server::process_event(event_t const& event)
  {
    boost::apply_visitor(Server_Event_Visitor{*this}, event);
  }
  void Server::on_physics_click(btCollisionObject const* obj)
  {
    Physics_Event_Decl* ptr =
      static_cast<Physics_Event_Decl*>(obj->getUserPointer());
    if(ptr)
    {
      // Check to see if we've past the time out for this event yet
      if(ptr->last_trigger.has_been(FloatSeconds(ptr->timeout)))
      {
        ptr->last_trigger.reset();
        // Fire an event because we have a valid physics event.
        Lua_Event event;
        event.type = Lua_Event::Physics;
        event.physics_decl = ptr;
        push_lua_event(event);
      }
    }
  }
  bool Server::poll_lua_event(Lua_Event& event)
  {
    return lua_event_queue_.poll_event(event);
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

  Net_Server::Net_Server(yojimbo::Allocator& allocator,
                         yojimbo::Transport& transport,
                         yojimbo::ClientServerConfig const& csconfig,
                         double time)
          : Server(allocator, transport, csconfig, time)
  {

  }
  yojimbo::MessageFactory* Net_Server::CreateMessageFactory(
          yojimbo::Allocator& allocator,
          yojimbo::ServerResourceType type,
          int client_index)
  {
    return YOJIMBO_NEW(allocator, Net_Message_Factory);
  }

  void Server::req_player()
  {
    // New id
    auto id = this->index_gen.get();

    // Be careful about reusing the memory for the player controller here.
    Player& player = at_id(this->players, id);
    player.controller.reset();
    player.controller.server = this;

    // New player, notify anyone who cares
    engine_->push_outgoing_event(New_Player_Event{id, true});
  }
  Player& Server::player(player_id id)
  {
    return players[id-1];
  }
  void Server::load_map(std::string const& filename)
  {
    log_i("Loading map '%'...", filename);

    // Load json file
    rapidjson::Document mapdoc;
    if(!load_json(mapdoc, filename + ".json"))
    {
      log_e("Failed to load map file: %.json", filename);
      return;
    }

    // Allocate on the heap so the address doesn't change, it might be
    // referenced in the physics or render component.
    auto map = std::make_unique<Map>();

    std::string err;
    if(load_map_json(*map, mapdoc, &err))
    {
      log_i("Successfully loaded map '%'", filename);
    }
    else
    {
      log_e("Failed to load map '%': %", filename, err);
    }

    // Record the pointer to the map
    auto map_observer = observer_ptr<Map>{map.get()};

    // Keep the map around in the server
    maps.push_back(std::move(map));

    // Queue the new map event. This should inform the client and a Server
    // routine that the map has been loaded. The client will probably upload
    // the data to GPU memory and the server will give bullet the data so we
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
  }

  void Scene::process_event(event_t const& event)
  {
    boost::apply_visitor(Scene_Event_Visitor{*this}, event);
  }
}

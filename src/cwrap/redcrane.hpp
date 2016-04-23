/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 *
 * C++ code + structs that will not be exposed to the C interface but will be
 * useful for its implementation.
 */
#include <cstdint>
#include <memory>

#include <boost/variant.hpp>

#include "../minilua.h"

#include "../common/timer.hpp"
#include "../common/id_map.hpp"
#include "../common/peer_ptr.hpp"

#include "../use/mesh_cache.h"
#include "../use/texture.h"

#include "../gfx/idriver.h"
#include "../gfx/camera.h"
#include "../gfx/mesh_chunk.h"

#include "../common/cache.h"

#include "../fps/camera_controller.h"

#include "../sdl_helper.h"

#include <boost/filesystem.hpp>

#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>

#ifndef REDC_REDCRANE_DECL_H
#define REDC_REDCRANE_DECL_H
extern "C"
{
  #include "../redcrane_decl.h"
}
#endif

// This doesn't have to be a macro
#define REDC_ASSERT_HAS_CLIENT(rce) \
REDC_ASSERT_MSG(rce->client != nullptr, "Client must be initialized")

namespace redc
{
  template <class T>
  inline Peer_Lock<T> lock_resource(void* ptr)
  {
    auto peer = (Peer_Ptr<T>*) ptr;
    return peer->lock();
  }

  struct Client;
  struct Map;

  struct Engine
  {
    Redc_Config config;
    boost::filesystem::path share_path;
    bool running = true;

    std::unique_ptr<gfx::Mesh_Cache> mesh_cache;

    Peer_Lock<Map> active_map;
    std::vector<Peer_Ptr<Map> > maps;

    std::unique_ptr<Client> client;
    //std::unique_ptr<Server> server;
  };

  struct Scene;

  struct Upload_Map_Mesh
  {
    using ptr_type = std::unique_ptr<gfx::Mesh_Chunk>;

    ptr_type operator()(ptr_type chunk, Map* map);

    // Make sure to set this!
    gfx::IDriver* driver;
  };

  struct Client
  {
    Client(redc::SDL_Init_Lock l) : sdl_raii(std::move(l)) {}

    redc::SDL_Init_Lock sdl_raii;

    // Make sure we put this at the top so it is uninitialized relatively after
    // we have to deallocate all the meshes, textures, etc.
    // in the game.
    std::unique_ptr<gfx::IDriver> driver;

    // Destruct all these together, if lua hasn't already.
    std::vector<Peer_Ptr<void> > peers;

    Cache<gfx::Mesh_Chunk, Upload_Map_Mesh, Map*> map_chunk;

    // TODO: Maybe keep track of every scene so lua doesn't have to deal with it
    // We should reserve some amount of memory for each scene so that we can
    // pass around pointers and no they won't suddenly become invalid.
  };

  struct Mesh_Object
  {
    Peer_Lock<gfx::Mesh_Chunk> chunk;
    glm::mat4 model;
  };

  struct Cam_Object
  {
    Cam_Object(gfx::Camera const& c) : cam(c), control() {}

    gfx::Camera cam;
    fps::Camera_Controller control;
  };

  struct Object
  {
    // This must match the order in the boost variant!!!
    enum Obj_Type : unsigned int
    {
      Mesh = 0,
      Cam = 1
    };

    boost::variant<Mesh_Object, Cam_Object> obj;

    // Non-owned parent, they should all be in the vector.
    Object* parent;
  };

  glm::mat4 object_model(Object const& obj);

  template <class Arr>
  inline auto at_id(Arr& arr, obj_id id) -> decltype(auto)
  {
    return arr[id-1];
  };

  struct Scene
  {
    // This is an unordered map that also keeps an active camera available to
    // us.
    Engine* engine;

    // Should always be a Obj_Type::Cam!! Can we enforce that with the type
    // system?
    obj_id active_camera;

    ID_Gen<obj_id> index_gen;
    std::array<Object, std::numeric_limits<obj_id>::max()> objs;

#ifdef REDC_LOG_FRAMES
    int16_t frame_count = 0;
    Timer<> frame_timer;
#endif
  };

  struct Map
  {
    Map(Indexed_Mesh_Data&& data) noexcept;
    Indexed_Mesh_Data render_mesh;
    btTriangleIndexVertexArray collis_mesh;
    btBvhTriangleMeshShape collis_shape;
  };
}

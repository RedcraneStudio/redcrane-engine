/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 *
 * C++ code + structs that will not be exposed to the C interface but will be
 * useful for its implementation.
 */
#pragma once
#include <cstdint>
#include <memory>

#include <boost/variant.hpp>

#include "../minilua.h"

#include "../common/timer.hpp"
#include "../common/id_map.hpp"
#include "../common/peer_ptr.hpp"

#include "../effects/envmap.h"

#include "../use/mesh_cache.h"
#include "../use/texture.h"

#include "../gfx/idriver.h"
#include "../gfx/camera.h"
#include "../gfx/mesh_chunk.h"
#include "../gfx/itexture.h"
#include "../gfx/asset_render.h"

#include "../common/cache.h"

#include "../fps/camera_controller.h"

#include "../sdl_helper.h"

#include <boost/filesystem.hpp>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "../map.h"
#include "../player.h"
#include "../server.h"
#include "../event.h"
#include "../common/reactor.h"

#include "soloud.h"
#include "soloud_wav.h"
#include "soloud_wavstream.h"

#define REDC_TRUE 1
#define REDC_FALSE 0

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

#define REDC_ASSERT_HAS_SERVER(rce) \
REDC_ASSERT_MSG(rce->server != nullptr, "Server must be initialized")

namespace redc
{
  template <class T>
  inline Peer_Lock<T> lock_resource(void* ptr)
  {
    auto peer = (Peer_Ptr<T>*) ptr;
    return peer->lock();
  }

  struct Client;
  struct Server;
  struct Map;
  struct Engine
  {
    Redc_Config config;
    boost::filesystem::path share_path;
    bool running = true;

    std::unique_ptr<gfx::Mesh_Cache> mesh_cache;

    std::unique_ptr<SoLoud::Soloud> audio;

    std::unique_ptr<Client> client;
    std::unique_ptr<Server> server;

    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point last_frame;

    void push_outgoing_event(Event event);
    bool poll_event(Event& event);
  private:
    Queue_Event_Source<Event> event_queue_;
  };

  template <class T>
  double time_since(T before) noexcept
  {
    T now = std::chrono::high_resolution_clock::now();
    using sec_t = std::chrono::duration<double, std::chrono::seconds::period>;
    return sec_t(now - before).count();
  }

  struct Scene;

  struct Client : public Event_Sink<Event>
  {
    Client(redc::SDL_Init_Lock l)
      : sdl_raii(std::move(l)), input_cfg(get_default_input_config()) {}

    redc::SDL_Init_Lock sdl_raii;

    Input_Config input_cfg;

    std::array<SoLoud::Wav, 5> step_sounds;

    // Make sure we put this at the top so it is uninitialized relatively after
    // we have to deallocate all the meshes, textures, etc.
    // in the game.
    std::unique_ptr<gfx::IDriver> driver;

    // Destruct all these together, if lua hasn't already.
    std::vector<Peer_Ptr<void> > peers;

    // Scenes
    std::vector<Peer_Ptr<Scene> > scenes;

    // The default shader scenes should use when an object doesn't have a
    // specific shader.
    std::unique_ptr<gfx::IShader> default_shader;

    // TODO: Maybe keep track of every scene so lua doesn't have to deal with it
    // We should reserve some amount of memory for each scene so that we can
    // pass around pointers and no they won't suddenly become invalid.

    void process_event(event_t const& event) override;
  };

  struct Lua_Event
  {
    enum
    {
      Physics,
      Map_Loaded
    } type;
    union
    {
      Physics_Event_Decl* physics_decl;
      Map* map;
    };
  };
  struct Server : public Server_Base
  {
    Server(Engine& eng);

    void req_player() override;
    Player& player(player_id id) override;

    void load_map(std::string const& map) override;

    std::unique_ptr<btDefaultCollisionConfiguration> bt_config;
    std::unique_ptr<btCollisionDispatcher> bt_dispatcher;
    std::unique_ptr<btDbvtBroadphase> bt_broadphase;
    std::unique_ptr<btSequentialImpulseConstraintSolver> bt_solver;

    std::unique_ptr<btDiscreteDynamicsWorld> bt_world;

    obj_id active_player;

    ID_Gen<obj_id> index_gen;
    std::array<Player, std::numeric_limits<obj_id>::max()> players;

    // Use a unique pointer so we can have pointers to maps.
    std::vector<std::unique_ptr<Map> > maps;

    void process_event(event_t const& event) override;
    void on_physics_click(btCollisionObject const* object);

    // Server events are distinctly different from engine standard events.
    bool poll_lua_event(Lua_Event& event);
    void push_lua_event(Lua_Event event) { lua_event_queue_.push(event); }
  private:
    Engine* engine_;

    Queue_Event_Source<Lua_Event> lua_event_queue_;
  };

  struct Mesh_Object
  {
    // TODO: To make this work with the map, make an aliasing constructor like
    // shared_ptr has for the peer_lock / peer_ptr.
    Peer_Lock<gfx::Mesh_Chunk> chunk;
    Peer_Lock<gfx::ITexture> texture;
    Peer_Lock<gfx::IShader> shader;
    glm::mat4 model;
  };

  struct Cam_Object
  {
    Cam_Object(gfx::Camera const& c) : cam(c), control(),follow_player(false){}

    gfx::Camera cam;
    fps::Camera_Controller control;

    bool follow_player;
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
    Object* parent = nullptr;
  };

  glm::mat4 object_model(Object const& obj);

  template <class Arr>
  inline auto at_id(Arr& arr, obj_id id) -> decltype(auto)
  {
    return arr[id-1];
  };

  struct Scene : public Event_Sink<Event>
  {
    // This is an unordered map that also keeps an active camera available to
    // us.
    Engine* engine = nullptr;

    // Should always be a Obj_Type::Cam!! Can we enforce that with the type
    // system?
    obj_id active_camera = 0;

    Player* active_player = nullptr;

    ID_Gen<obj_id> index_gen;
    std::array<Object, std::numeric_limits<obj_id>::max()> objs;

#ifdef REDC_LOG_FRAMES
    int16_t frame_count = 0;
    Timer<> frame_timer;
#endif

    // Use for the rendering the map, and ideally everything, so that it works
    // properly.
    gfx::Rendering_State render_state;

    // Maps are completely referenced in the engine, so we don't need peer locks
    // here.
    observer_ptr<Map> active_map;

    std::unique_ptr<gfx::ITexture> crosshair;
    std::unique_ptr<gfx::IBuffer> ch_buf;
    std::unique_ptr<gfx::IMesh> ch_mesh;
    std::unique_ptr<gfx::IShader> ch_shader;

    effects::Envmap_Effect envmap;

    int step_i = 0;

    Input cur_input;

    void process_event(event_t const& event) override;
  };
}

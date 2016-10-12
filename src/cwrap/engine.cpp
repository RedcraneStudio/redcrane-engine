/*
 * Copyright (c) 2016 Luke San Antonio
 * All rights reserved.
 *
 * This file provides the implementation for the engine's C interface,
 * specifically tailored for LuaJIT's FFI facilities.
 *
 * General engine stuff
 */

#include "redcrane.hpp"

#include "../common/version.h"

#include "../assets/load_dir.h"
#include "../gfx/gl/driver.h"

#include <thread>
#include <chrono>

using namespace redc;

namespace
{
  void redc_lua_log(Log_Severity s, const char *msg)
  {
    redc::log(s, msg);
  }
}

extern "C"
{
  // See redcrane.lua

  void redc_log_d(const char* str)
  {
    redc_lua_log(Log_Severity::Debug, str);
  }
  void redc_log_i(const char* str)
  {
    redc_lua_log(Log_Severity::Info, str);
  }
  void redc_log_w(const char* str)
  {
    redc_lua_log(Log_Severity::Warning, str);
  }
  void redc_log_e(const char* str)
  {
    redc_lua_log(Log_Severity::Error, str);
  }

  void redc_ms_sleep(unsigned long ms)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
  }

  float redc_cur_time()
  {
    return SDL_GetTicks() / 1000.0f;
  }

  void* redc_init_engine(Redc_Config cfg)
  {
    // Switch into the current working directory requested by the mod
    if(!exists(boost::filesystem::path(cfg.cwd)))
    {
      log_e("Failed to enter requested '%' directory", cfg.cwd);
      return nullptr;
    }

    auto eng = new Engine;
    eng->config = cfg;
    eng->share_path = assets::share_path();

    eng->audio = std::make_unique<SoLoud::Soloud>();
    eng->audio->init();
    // Play at full volume.
    eng->audio->setPostClipScaler(1.0f);

    eng->mesh_cache =
            std::make_unique<gfx::Mesh_Cache>(eng->share_path / "obj",
                                              eng->share_path / "obj_cache");

    log_i("Initialized the Red Crane Engine alpha version %.%.% (Mod: %)",
          REDC_ENGINE_VERSION_MAJOR, REDC_ENGINE_VERSION_MINOR,
          REDC_ENGINE_VERSION_PATCH, cfg.mod_name);

    eng->start_time = std::chrono::high_resolution_clock::now();
    eng->last_frame = eng->start_time;

    return eng;
  }
  void redc_init_client(void* eng)
  {
    auto rce = (Engine*) eng;

    auto sdl_init_raii_lock =
            redc::init_sdl(rce->config.mod_name, {1000,1000}, false, false);
    auto sdl_window = sdl_init_raii_lock.window;

    rce->client = std::make_unique<Client>(std::move(sdl_init_raii_lock));

    SDL_SetRelativeMouseMode(SDL_TRUE);

    int x, y;
    SDL_GetWindowSize(sdl_window, &x, &y);

    rce->client->driver = std::make_unique<gfx::gl::Driver>(Vec<int>{x,y});

    rce->client->driver->set_clear_color(colors::clear_black);

    // Load default shader, etc.
    auto df_shade = rce->client->driver->make_shader_repr();

    // TODO: Load shaders like we load mesh. Right now is bad
    auto basic_shade_path = rce->share_path / "shader" / "basic";
    load_vertex_file(*df_shade, (basic_shade_path / "vs.glsl").string());
    load_fragment_file(*df_shade, (basic_shade_path / "fs.glsl").string());

    df_shade->link();

    using namespace gfx::tags;
    df_shade->set_var_tag(model_tag, "model");
    df_shade->set_var_tag(view_tag, "view");
    df_shade->set_var_tag(proj_tag, "proj");

    df_shade->set_var_tag(dif_tex_tag, "diffuse");
    df_shade->set_integer(dif_tex_tag, 0);

    // TODO: Put this in a lua script so we don't have to do this.
    df_shade->tag_var("light_pos");
    // Set light position, maybe put standardize this tag?
    df_shade->set_vec3("light_pos", glm::vec3(0.0f, 5.0f, 0.0f));

    // Make it the default
    rce->client->driver->use_shader(*df_shade);

    // Keep the shader alive for the lifetime of the client.
    rce->client->default_shader = std::move(df_shade);

    rce->client->step_sounds[0].load("../assets/snd/walk_variation_0.wav");
    rce->client->step_sounds[1].load("../assets/snd/walk_variation_1.wav");
    rce->client->step_sounds[2].load("../assets/snd/walk_variation_2.wav");
    rce->client->step_sounds[3].load("../assets/snd/walk_variation_3.wav");
    rce->client->step_sounds[4].load("../assets/snd/walk_variation_4.wav");

    log_i("Initializing client subsystem ... Successful");
  }
  void redc_init_server(void* eng)
  {
    auto rce = (Engine*) eng;
    rce->server = std::make_unique<Server>(*rce);

    log_i("Initializing server subsystem ... Successful");
  }
  void redc_uninit_engine(void* eng)
  {
    auto rce = (Engine*) eng;
    rce->audio->deinit();
    delete rce;
  }

  void redc_step_engine(void* eng)
  {
    auto rce = (Engine*) eng;
    Event event;
    while(rce->poll_event(event))
    {
      // Please branch predictor save me
      if(rce->client) rce->client->process_event(event);
      if(rce->server) rce->server->process_event(event);
    }
  }

  bool redc_running(void *eng)
  {
    auto rce = (Engine*) eng;
    return rce->running;
  }

  const char* redc_get_asset_path(void* eng)
  {
    // Will this ever change mid-execution?
    auto rce = (Engine*) eng;
    return rce->share_path.string().c_str();
  }
  void redc_window_swap(void* eng)
  {
    auto engine = (Engine*) eng;
    if(engine->client)
    {
      SDL_GL_SwapWindow(engine->client->sdl_raii.window);
    }
  }

  void redc_gc(void* eng)
  {
    auto rce = (Engine*) eng;

    auto is_null = [](Peer_Ptr<void> const& peer)
    {
      return peer.get() == nullptr;
    };

    // Go through the vector of peer pointers and removed deallocated ones.
    using std::begin; using std::end;

    if(rce->client)
    {
      auto& client = *rce->client;

      auto peer_end = std::remove_if(begin(client.peers), end(client.peers),
                                     is_null);
      client.peers.erase(peer_end, end(client.peers));
    }
  }
}

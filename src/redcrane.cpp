/*
 * Copyright (c) 2016 Luke San Antonio
 * All rights reserved.
 *
 * This file provides the implementation for the engine's C interface,
 * specifically tailored for LuaJIT's FFI facilities.
 */

#include <cstdint>

#include "redcrane.hpp"

#include "common/log.h"
#include "common/debugging.h"

#include "use/mesh.h"
#include "use/mesh_cache.h"

#include "gfx/gl/driver.h"
#include "gfx/camera.h"

#include "assets/load_dir.h"

#include "sdl_helper.h"

extern "C"
{
#include "redcrane_decl.h"
}

using namespace redc;

namespace
{
  void redc_lua_log(Log_Severity s, const char *msg)
  {
    redc::log(s, "(Mod) %", msg);
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

  void* redc_init_engine(Redc_Config cfg)
  {
    SDL_Init_Lock sdl_init_raii_lock{cfg.window_title, {1000,1000}, false, false};
    auto sdl_window = sdl_init_raii_lock.window;

    SDL_SetRelativeMouseMode(SDL_TRUE);

    int x, y;
    SDL_GetWindowSize(sdl_window, &x, &y);

    auto eng = new Engine{nullptr, nullptr, sdl_window};
    eng->driver = std::make_unique<gfx::gl::Driver>(Vec<int>{x,y});

    auto share_path = assets::share_path();
    eng->mesh_cache =
            std::make_unique<gfx::Mesh_Cache>(share_path / "obj",
                                              share_path / "obj_cache");

    return eng;
  }
  void redc_uninit_engine(void* eng)
  {
    auto rce = (redc::Engine*) eng;
    delete rce;
  }

  bool redc_running(void* eng)
  {
    return ((redc::Engine*) eng)->running;
  }

  void redc_step(void* eng)
  {
    auto engine = (redc::Engine*) eng;
  }

  // See mesh_pool.lua
  void* redc_load_mesh(void* engine, const char* str)
  {
    // TODO: Load many static objects into the same mesh.
    auto rce = (redc::Engine*) engine;

    auto res = gfx::load_mesh(*rce->driver,
                              *rce->mesh_cache,
                              {std::string{str}, false});

    // Allocate a mesh_chunk to store it
    auto chunk = new gfx::Mesh_Chunk;
    *chunk = copy_mesh_chunk_move_mesh(res.chunk);
    return chunk;
  }
  void redc_unload_mesh(void* mesh)
  {
    auto chunk = (gfx::Mesh_Chunk*) mesh;
    delete chunk;
  }

  // See scene.lua
  void* redc_make_scene(void* engine)
  {
    auto sc = new redc::Scene;
    sc->engine = (redc::Engine*) engine;
    return sc;
  }
  void redc_unmake_scene(void* scene)
  {
    auto sc = (redc::Scene*) scene;
    delete sc;
  }

  uint16_t redc_scene_add_camera(void* sc, const char* tp)
  {
    // For the moment, this is the only kind of camera we support

    std::function<gfx::Camera (gfx::IDriver const&)> cam_func;

    if(strcmp(tp, "fps") == 0)
    {
      // Ayy we got a camera
      cam_func = redc::gfx::make_fps_camera;
    }
    else
    {
      log_w("Invalid camera type '%' so making an fps camera", tp);
    }


    // The first camera will be set as active automatically by Active_Map from
    // id_map.hpp.
    auto scene = (redc::Scene*) sc;
    auto id = scene->cams.insert(cam_func(*scene->engine->driver));

    // Return the id
    return id;
  }

  uint16_t redc_scene_get_active_camera(void* sc)
  {
    auto scene = (redc::Scene*) sc;

    return scene->cams.active_element();
  }
  void redc_scene_activate_camera(void* sc, uint16_t cam)
  {
    // We have a camera
    auto scene = (redc::Scene*) sc;

    scene->cams.active_element(cam);
  }

  void redc_scene_attach(void* sc, void* mesh, void* parent)
  {
    log_e("I don't know how to attach a mesh to an object");
  }

  void redc_draw_mesh(void* engine, void* mesh)
  {
    gfx::Mesh_Chunk* chunk = (gfx::Mesh_Chunk*) mesh;
    gfx::render_chunk(*chunk);
  }

  void redc_swap_window(void* engine)
  {
    auto rce = (redc::Engine*) engine;
    SDL_GL_SwapWindow(rce->window);
  }
}

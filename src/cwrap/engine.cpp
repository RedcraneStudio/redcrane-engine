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

#include "../assets/load_dir.h"
#include "../gfx/gl/driver.h"

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
    auto sdl_init_raii_lock =
            redc::init_sdl(cfg.window_title, {1000,1000}, false, false);
    auto sdl_window = sdl_init_raii_lock.window;

    SDL_SetRelativeMouseMode(SDL_TRUE);

    int x, y;
    SDL_GetWindowSize(sdl_window, &x, &y);

    auto eng = new Engine{std::move(sdl_init_raii_lock), nullptr, nullptr};
    eng->driver = std::make_unique<gfx::gl::Driver>(Vec<int>{x,y});

    auto share_path = assets::share_path();
    eng->mesh_cache =
            std::make_unique<gfx::Mesh_Cache>(share_path / "obj",
                                              share_path / "obj_cache");

    // Load default shader, etc.
    auto df_shade = eng->driver->make_shader_repr();

    // TODO: Load shaders like we load mesh. Right now is bad
    auto basic_shade_path = share_path / "shader" / "basic";
    df_shade->load_vertex_part((basic_shade_path / "vs.glsl").native());
    df_shade->load_fragment_part((basic_shade_path / "fs.glsl").native());

    df_shade->set_model_name("model");
    df_shade->set_view_name("view");
    df_shade->set_projection_name("proj");

    // Make it the default
    eng->driver->use_shader(*df_shade);

    // Make sure we don't delete it later by linking its lifetime with that of
    // the engines.
    eng->shaders.push_back(std::move(df_shade));

    return eng;
  }
  void redc_uninit_engine(void* eng)
  {
    auto rce = (redc::Engine*) eng;
    delete rce;
  }
  const char* redc_get_asset_path(void*)
  {
    // Will this ever change mid-execution?
    static auto str = redc::assets::share_path().native();
    return str.data();
  }
  void redc_window_swap(void* eng)
  {
    auto engine = (redc::Engine*) eng;
    SDL_GL_SwapWindow(engine->sdl_raii.window);
  }
}

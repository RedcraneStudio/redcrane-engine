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
      return gfx::camera_model_matrix(cam.cam);
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
}

using namespace redc;

#define CHECK_ID(id) \
        REDC_ASSERT_MSG(id != 0, "No more room for any more objects"); \
        if(id == 0) return id;

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
    std::shared_ptr<gfx::Shader> df_shade = eng->driver->make_shader_repr();

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

    auto id = scene->index_gen.get();
    CHECK_ID(id);
    auto& obj = scene->objs[id-1];
    obj.obj = Cam_Object{cam_func(*scene->engine->driver)};

    // We can be sure at this point the id is non-zero (because of CHECK_ID).

    // If this is our first camera
    if(!scene->active_camera) scene->active_camera = id;

    // Return the id
    return id;
  }

  uint16_t redc_scene_get_active_camera(void* sc)
  {
    auto scene = (redc::Scene*) sc;

    // This will be zero when there isn't an active camera.
    return scene->active_camera;
  }
  void redc_scene_activate_camera(void* sc, uint16_t cam)
  {
    if(!cam)
    {
      log_w("Cannot make an invalid object the active camera, "
            "ignoring request");
      return;
    }

    // We have a camera
    auto scene = (redc::Scene*) sc;

    if(scene->objs[cam].obj.which() == Object::Cam)
    {
      scene->active_camera = cam;
    }
    else
    {
      log_w("Cannot make non-camera the active camera, ignoring request");
    }
  }

  uint16_t redc_scene_attach(void* sc, void* ms, uint16_t parent)
  {
    auto scene = (redc::Scene*) sc;
    auto mesh = (redc::gfx::Mesh_Chunk*) ms;

    auto id = scene->index_gen.get();
    CHECK_ID(id);
    scene->objs[id-1].obj = Mesh_Object{gfx::copy_mesh_chunk_share_mesh(*mesh),
                                        glm::mat4(1.0f)};
    if(parent)
    {
      scene->objs[id-1].parent = &scene->objs[parent];
    }
  }
}

/*
 * Copyright (c) 2016 Luke San Antonio
 * All rights reserved.
 *
 * This file provides the implementation for the engine's C interface,
 * specifically tailored for LuaJIT's FFI facilities.
 *
 * Scene stuff
 */

#include "redcrane.hpp"

#define CHECK_ID(id) \
        REDC_ASSERT_MSG(id != 0, "No more room for any more objects"); \
        if(id == 0) return id;

using namespace redc;

extern "C"
{
  // See scene.lua
  void *redc_make_scene(void *engine)
  {
    auto sc = new redc::Scene;
    sc->engine = (redc::Engine *) engine;
    return sc;
  }
  void redc_unmake_scene(void *scene)
  {
    auto sc = (redc::Scene *) scene;
    delete sc;
  }

  uint16_t redc_scene_add_camera(void *sc, const char *tp)
  {
    // For the moment, this is the only kind of camera we support

    std::function <gfx::Camera(gfx::IDriver const&)> cam_func;

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
    auto scene = (redc::Scene *) sc;

    auto id = scene->index_gen.get();
    CHECK_ID(id);
    auto &obj = scene->objs[id - 1];
    obj.obj = Cam_Object{cam_func(*scene->engine->driver)};

    // We can be sure at this point the id is non-zero (because of CHECK_ID).

    // If this is our first camera
    if(!scene->active_camera) scene->active_camera = id;

    // Return the id
    return id;
  }

  uint16_t redc_scene_get_active_camera(void *sc)
  {
    auto scene = (redc::Scene *) sc;

    // This will be zero when there isn't an active camera.
    return scene->active_camera;
  }
  void redc_scene_activate_camera(void *sc, uint16_t cam)
  {
    if(!cam)
    {
      log_w("Cannot make an invalid object the active camera, "
                    "ignoring request");
      return;
    }

    // We have a camera
    auto scene = (redc::Scene *) sc;

    if(scene->objs[cam].obj.which() == Object::Cam)
    {
      scene->active_camera = cam+1;
    }
    else
    {
      log_w("Cannot make non-camera the active camera, ignoring request");
    }
  }

  uint16_t redc_scene_attach(void *sc, void *ms, uint16_t parent)
  {
    auto scene = (redc::Scene *) sc;
    auto mesh = (redc::gfx::Mesh_Chunk *) ms;

    auto id = scene->index_gen.get();
    CHECK_ID(id);
    scene->objs[id - 1].obj = Mesh_Object{gfx::copy_mesh_chunk_share_mesh(*mesh),
                                          glm::mat4(1.0f)};
    if(parent)
    {
      scene->objs[id - 1].parent = &scene->objs[parent];
    }
  }

  bool redc_running(void *eng)
  {
    return ((redc::Engine *) eng)->running;
  }

  void redc_scene_step(void *sc)
  {
    auto scene = (redc::Scene *) sc;

    Cam_Object* active_camera;
    if(scene->active_camera)
    {
      active_camera =
              &boost::get<Cam_Object>(scene->objs[scene->active_camera-1].obj);
    }

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
      // If we already used the event, bail.
      //if(collect_input(input, event, input_cfg)) continue;

      // Otherwise
      switch(event.type)
      {
        case SDL_QUIT:
          scene->engine->running = false;
          break;
        case SDL_MOUSEMOTION:
          if(active_camera)
          {
            active_camera->control.apply_delta_yaw(active_camera->cam,
                    event.motion.xrel / 1000.0f);

            active_camera->control.apply_delta_pitch(active_camera->cam,
                    event.motion.yrel / 1000.0f);
          }
          break;
        case SDL_KEYDOWN:
          //if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) running = false;
          break;
        default:
          break;
      }
    }
  }

  void redc_scene_render(void *sc)
  {
    auto scene = (redc::Scene *) sc;

    // Make sure we have an active camera
    if(!scene->active_camera)
    {
      log_e("No active camera; cannot render scene");
      return;
    }

    // Load the active camera
    auto active_camera =
            boost::get<Cam_Object>(scene->objs[scene->active_camera - 1].obj);
    gfx::use_camera(*scene->engine->driver, active_camera.cam);

    // Clear the screen
    scene->engine->driver->clear();

    // Find active shader.
    auto active_shader = scene->engine->driver->active_shader();

    // i is the loop counter, id is our current id.
    // Loop however many times as we have ids.
    int cur_id = 0;
    for(int i = 0; i < scene->index_gen.reserved(); ++i);
    {
      // Check to make sure the current id hasn't been removed.
      // Remember to add one

      // Increment the current id until we find one that is valid. Technically
      // we could just check if it hasn't been removed because we shouldn't
      // get far enough to exceed count but whatever this makes more semantic
      // sense. Then again if we exceed count_ we could enter a loop where we
      // exit only at overflow.
      while(!scene->index_gen.is_valid((cur_id + 1))) { ++cur_id; }

      // If the above scenario becomes an issue, replace !is_valid with
      // is_removed and check if it's valid here. If it hasn't been removed
      // but isn't valid we went to far, so exit early. I'm not doing that here
      // because I don't think it will be an issue.

      auto &obj = scene->objs[cur_id];
      if(obj.obj.which() == Object::Cam)
      {
        // Debugging enabled? Render cameras in some way?
      }
      else if(obj.obj.which() == Object::Mesh)
      {
        auto mesh_obj = boost::get<Mesh_Object>(obj.obj);

        // Find out the model
        auto model = object_model(obj);

        gfx::render_chunk(mesh_obj.chunk);
      }
    }
  }
}

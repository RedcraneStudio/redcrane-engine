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
  void *redc_make_scene(void *eng)
  {
    auto engine = (Engine*) eng;
    REDC_ASSERT_HAS_CLIENT(engine);

    auto sc = new Peer_Ptr<Scene>(new Scene);
    sc->get()->engine = engine;

    engine->client->peers.push_back(sc->peer());
    return sc;
  }
  void redc_unmake_scene(void *scene)
  {
    auto sc = (Peer_Ptr<Scene> *) scene;
    delete sc;
  }

  obj_id redc_scene_add_camera(void *sc, const char *tp)
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
    auto scene = lock_resource<redc::Scene>(sc);
    REDC_ASSERT_HAS_CLIENT(scene->engine);

    auto id = scene->index_gen.get();
    CHECK_ID(id);

    auto &obj = at_id(scene->objs, id);
    obj.obj = Cam_Object{cam_func(*scene->engine->client->driver)};

    // We can be sure at this point the id is non-zero (because of CHECK_ID).

    // If this is our first camera
    if(!scene->active_camera) scene->active_camera = id;

    // Return the id
    return id;
  }

  obj_id redc_scene_get_active_camera(void *sc)
  {
    auto scene = lock_resource<redc::Scene>(sc);

    // This will be zero when there isn't an active camera.
    return scene->active_camera;
  }
  void redc_scene_activate_camera(void *sc, obj_id cam)
  {
    if(!cam)
    {
      log_w("Cannot make an invalid object the active camera, "
                    "ignoring request");
      return;
    }

    // We have a camera
    auto scene = lock_resource<redc::Scene>(sc);

    if(at_id(scene->objs, cam).obj.which() == Object::Cam)
    {
      scene->active_camera = cam;
    }
    else
    {
      log_w("Cannot make non-camera the active camera, ignoring request");
    }
  }

  obj_id redc_scene_attach(void *sc, void *ms, obj_id parent)
  {
    auto scene = lock_resource<redc::Scene>(sc);

    auto id = scene->index_gen.get();
    CHECK_ID(id);

    auto mesh = lock_resource<gfx::Mesh_Chunk>(ms);

    auto& obj = at_id(scene->objs, id);
    obj.obj = Mesh_Object{std::move(mesh), glm::mat4(1.0f)};

    if(parent)
    {
      obj.parent = &at_id(scene->objs, parent);
    }

    return id;
  }

  void redc_scene_step(void *sc)
  {
    auto scene = lock_resource<redc::Scene>(sc);

    Cam_Object* active_camera;
    if(scene->active_camera)
    {
      active_camera = &boost::get<Cam_Object>(at_id(scene->objs,
                                                    scene->active_camera).obj);
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
    auto scene = lock_resource<redc::Scene>(sc);

    // We got to have a client component or whatever-you-want-to-call-it
    // available.
    REDC_ASSERT_HAS_CLIENT(scene->engine);

#ifdef REDC_LOG_FRAMES
    ++scene->frame_count;
    if(scene->frame_timer.has_been(std::chrono::seconds(1)))
    {
      log_d("fps: %", scene->frame_count);
      scene->frame_count = 0;
      scene->frame_timer.reset();
    }
#endif

    // Make sure we have an active camera
    if(!scene->active_camera)
    {
      log_e("No active camera; cannot render scene");
      return;
    }

    // Load the active camera
    auto active_camera =
            boost::get<Cam_Object>(at_id(scene->objs,
                                         scene->active_camera).obj);

    gfx::use_camera(*scene->engine->client->driver, active_camera.cam);

    // Clear the screen
    scene->engine->client->driver->clear();

    // Find active shader.
    auto active_shader = scene->engine->client->driver->active_shader();

    // Given a map, generate a mesh chunk for the map.
    scene->engine->client->map_chunk.set_dependency<0>(
            scene->engine->active_map.get()
    );

    // Render the map
    active_shader->set_model(glm::mat4(1.0f));
    gfx::render_chunk(*scene->engine->client->map_chunk.cache());

    // i is the loop counter, id is our current id.
    // Loop however many times as we have ids.
    int cur_id = 0;
    for(int i = 0; i < scene->index_gen.reserved(); ++i)
    {
      // Check to make sure the current id hasn't been removed.
      // Remember to add one

      // Increment the current id until we find one that is valid. Technically
      // we could just check if it hasn't been removed because we shouldn't
      // get far enough to exceed count but whatever this makes more semantic
      // sense. Then again if we exceed count_ we could enter a loop where we
      // exit only at overflow.

      // TODO: Things that are confusing about this design
      // - Ids
      // Use of peer pointers vs. raw pointers vs. non-zero based ids
      // Use locks to simplify this!
      // Sandboxing
      // Networking - Lua
      // Write about significance of LLVM considering many new languages come
      // out that could be better for more niche things.

      // Put the increment in the expression because we always want to be
      // incrementing the counter, otherwise we risk rendering the same object
      // many times
      while(!scene->index_gen.is_valid((++cur_id)));

      // If the above scenario becomes an issue, replace !is_valid with
      // is_removed and check if it's valid here. If it hasn't been removed
      // but isn't valid we went to far, so exit early. I'm not doing that here
      // because I don't think it will be an issue.

      auto &obj = at_id(scene->objs, cur_id);
      if(obj.obj.which() == Object::Cam)
      {
        // Debugging enabled? Render cameras in some way?
      }
      else if(obj.obj.which() == Object::Mesh)
      {
        auto mesh_obj = boost::get<Mesh_Object>(obj.obj);

        // Find out the model
        auto model = object_model(obj);
        scene->engine->client->driver->active_shader()->set_model(model);

        gfx::render_chunk(*mesh_obj.chunk);
      }
    }
  }
}

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
#include <glm/gtc/matrix_transform.hpp>

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

    sc->get()->envmap.init(*engine->client->driver);

    // Add a crosshair to the camera, this is only used though if there is a
    // camera following the player which is why we can go ahead and add it now,
    // no problem.
    // TODO: Fix filename
    sc->get()->crosshair = gfx::load_texture(*engine->client->driver,
                                             "../assets/tex/crosshair.png");
    engine->client->driver->bind_texture(*sc->get()->crosshair, 1);

    {
      // Set up the mesh shader
      sc->get()->ch_mesh = engine->client->driver->make_mesh_repr();

      // Initialize mesh data
      std::array<float, 24> crosshair_data = {
        0.49f, 0.51f, 0.0f, 1.0f,
        0.51f, 0.51f, 1.0f, 1.0f,
        0.49f, 0.49f, 0.0f, 0.0f,

        0.51f, 0.51f, 1.0f, 1.0f,
        0.51f, 0.49f, 1.0f, 0.0f,
        0.49f, 0.49f, 0.0f, 0.0f
      };

      auto sz = sizeof(float) * crosshair_data.size();
      auto buf =sc->get()->ch_mesh->allocate_buffer(sz, Usage_Hint::Draw,
                                                           Upload_Hint::Static);
      sc->get()->ch_mesh->buffer_data(buf, 0, sz, &crosshair_data[0]);
      // 2D Positions
      sc->get()->ch_mesh->format_buffer(buf, 0, 2, Buffer_Format::Float,
                                               4 * sizeof(float), 0);
      sc->get()->ch_mesh->enable_vertex_attrib(0);

      // UV coordinates
      sc->get()->ch_mesh->format_buffer(buf, 1, 2, Buffer_Format::Float,
                                               4 * sizeof(float),
                                               2 * sizeof(float));
      sc->get()->ch_mesh->enable_vertex_attrib(1);

      sc->get()->ch_mesh->set_primitive_type(Primitive_Type::Triangle);

      // Set up the HUD shader
      sc->get()->ch_shader = engine->client->driver->make_shader_repr();
      load_vertex_file(*sc->get()->ch_shader, "../assets/shader/hud/vs.glsl");
      load_fragment_file(*sc->get()->ch_shader, "../assets/shader/hud/fs.glsl");

      sc->get()->ch_shader->link();

      sc->get()->ch_shader->set_var_tag(gfx::tags::proj_tag, "ortho");
      sc->get()->ch_shader->set_mat4(gfx::tags::proj_tag,
                                     glm::ortho(0.0f, 1.0f, 0.0f, 1.0f,
                                                -1.0f, 1.0f));

      sc->get()->ch_shader->set_var_tag(gfx::tags::dif_tex_tag, "tex");
      sc->get()->ch_shader->set_integer(gfx::tags::dif_tex_tag, 1);
    }
    engine->client->scenes.push_back(sc->peer());
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

    std::function <gfx::Camera(Vec<int> size)> cam_func;

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

    auto win_size = scene->engine->client->driver->window_extents();
    obj.obj = Cam_Object{cam_func(win_size)};

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
  void redc_scene_set_active_camera(void *sc, obj_id cam)
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

  void redc_scene_camera_set_follow_player(void *sc, obj_id cam, bool en)
  {
    auto scene = lock_resource<Scene>(sc);

    auto& object = at_id(scene->objs, cam);
    if(object.obj.which() != Object::Cam)
    {
      log_w("Cannot set non-camera object as player camera");
    }
    else
    {
      boost::get<Cam_Object>(object.obj).follow_player = en;
    }
  }

  obj_id redc_scene_add_mesh(void *sc, void *ms)
  {
    auto scene = lock_resource<redc::Scene>(sc);

    auto id = scene->index_gen.get();
    CHECK_ID(id);

    auto mesh = lock_resource<gfx::Mesh_Chunk>(ms);

    at_id(scene->objs, id) =
      {Mesh_Object{std::move(mesh), {}, {}, glm::mat4(1.0f)}, nullptr};

    return id;
  }

  void redc_scene_set_parent(void* sc, obj_id obj, obj_id parent)
  {
    auto scene = lock_resource<Scene>(sc);

    // Set parent of this object
    // We could just use ids for the parent reference too but it makes things
    // a little easier on our end to manage.
    at_id(scene->objs, obj).parent = &at_id(scene->objs, parent);
  }

  void redc_scene_object_set_texture(void *sc, obj_id obj, void *tex)
  {
    auto scene = lock_resource<Scene>(sc);

    auto& object = at_id(scene->objs, obj);
    if(object.obj.which() != Object::Mesh)
    {
      log_w("Cannot set texture of a non-mesh object");
    }
    else
    {
      auto texture = (Peer_Ptr<Texture>*) tex;
      boost::get<Mesh_Object>(object.obj).texture = texture->lock();
    }
  }

  void redc_scene_step(void *sc)
  {
    auto scene = lock_resource<redc::Scene>(sc);

    // Find the engine
    auto engine = scene->engine;

    Cam_Object* active_camera;
    if(scene->active_camera)
    {
      active_camera = &boost::get<Cam_Object>(at_id(scene->objs,
                                                    scene->active_camera).obj);
    }

    if(active_camera->follow_player && scene->active_player)
    {
      // Set the camera to the player.
      active_camera->cam.fp.pos = scene->active_player->controller.get_cam_pos();
    }

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
      // If we already used the event, bail.
      if(collect_input(scene->cur_input, event,
                       engine->client->input_cfg)) continue;

      // Otherwise
      switch(event.type)
      {
        case SDL_QUIT:
          scene->engine->running = false;
          break;
        case SDL_MOUSEMOTION:
        {
          auto dy = event.motion.xrel / 1000.0f;
          auto dp = event.motion.yrel / 1000.0f;
          if(scene->active_player)
          {
            scene->active_player->controller.apply_delta_yaw(dy);
            scene->active_player->controller.apply_delta_pitch(dp);
          }
          if(active_camera)
          {
            // TODO: Better solution: Get the camera orientation and position
            // from the player controller!
            active_camera->control.apply_delta_yaw(active_camera->cam, dy);
            active_camera->control.apply_delta_pitch(active_camera->cam, dp);
          }
          break;
        }
        default:
          break;
      }
    }

    // The controller has access to input, step the simulation
    engine->server->bt_world->stepSimulation(time_since(engine->last_frame), 10);
    engine->last_frame = std::chrono::high_resolution_clock::now();

    Player_Event player_event;
    while(scene->active_player->controller.poll_event(player_event))
    {
      switch(player_event.type)
      {
      case Player_Event::Footstep:
        engine->audio->play(engine->client->step_sounds[scene->step_i++ % 5]);
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

    // Clear the screen
    scene->engine->client->driver->clear();

    // Load the active camera now, since we just activated the proper shader
    auto active_camera =
            boost::get<Cam_Object>(at_id(scene->objs,
                                         scene->active_camera).obj);

    // Render the environment map
    scene->envmap.render(*scene->engine->client->driver, active_camera.cam);

    // Render the current / active map with the default shader
    auto& default_shader = scene->engine->client->default_shader;
    scene->engine->client->driver->use_shader(*default_shader);

    gfx::use_camera(*scene->engine->client->driver, active_camera.cam);

    scene->engine->client->driver->bind_texture(*scene->lightmap, 0);

    using namespace gfx::tags;

    // Making the function think OpenGL state *hasn't* changed is a dangerous
    // assumption we can't make.
    Rendering_State render_state;
    render_asset(scene->active_map->render->asset, active_camera.cam,
                 render_state);

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

        // Select a shader to use, either default or mesh-specific.
        auto shader = scene->engine->client->default_shader.get();
        if(mesh_obj.shader) shader = mesh_obj.shader.get();
        scene->engine->client->driver->use_shader(*shader);

        // TODO: Make this easier to do, I had to avoid the use_camera function
        // because it went through the driver. The thing is, with our new shader
        // interface it's easier to deal with shaders directly so use_camera
        // just got less useful.
        using namespace gfx::tags;
        shader->set_mat4(proj_tag, camera_proj_matrix(active_camera.cam));
        shader->set_mat4(view_tag, camera_view_matrix(active_camera.cam));

        // Find out the model
        auto model = object_model(obj);
        shader->set_mat4(model_tag, model);

        gfx::render_chunk(*mesh_obj.chunk);
      }
    }

    // Render the crosshair only if the active camera is a camera that follows
    // the player.
    scene->engine->client->driver->blending(true);
    scene->engine->client->driver->write_depth(false);
    scene->engine->client->driver->depth_test(false);
    scene->engine->client->driver->use_shader(*scene->ch_shader);
    scene->ch_mesh->draw_arrays(0, 6);
    scene->engine->client->driver->blending(false);
    scene->engine->client->driver->write_depth(true);
    scene->engine->client->driver->depth_test(true);
  }
}

/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#include "assets/load_dir.h"
#include "assets/minigltf.h"
#include "gfx/gl/driver.h"
#include "gfx/camera.h"
#include "sdl_helper.h"
#include "common/log.h"
#include "use/mesh.h"
#include "gfx/scene.h"
#include "gfx/asset_render.h"
#include "gfx/deferred.h"
#include "effects/envmap.h"
#include "glm/gtc/matrix_transform.hpp"
#include <boost/filesystem.hpp>
#include <chrono>

template <class T>
double time_since(T before) noexcept
{
  T now = std::chrono::high_resolution_clock::now();
  using sec_t = std::chrono::duration<double, std::chrono::seconds::period>;
  return sec_t(now - before).count();
}

int main(int argc, char** argv)
{
  using namespace redc;

  // Initialize logger
  Scoped_Log_Init log_init_raii_lock{};
  set_out_log_level(Log_Severity::Debug);

  if(argc < 2)
  {
    log_e("Usage: % filename.gltf", argv[0]);
    return EXIT_FAILURE;
  }

  // Go into asset dir
  auto share_path = assets::share_path();
  boost::filesystem::current_path(share_path);

  auto sdl_init_raii_lock =
          redc::init_sdl("Deferred Rendering Test", {1000,1000}, false, false);

  auto sdl_window = sdl_init_raii_lock.window;

  int x, y;
  SDL_GetWindowSize(sdl_window, &x, &y);
  gfx::gl::Driver driver(Vec<int>{x,y});
  driver.set_clear_color(colors::clear_black);

  effects::Envmap_Effect envmap;
  envmap.init(driver);

  // Set up deferred shader

  gfx::Output_Interface fb_out;

  gfx::Attachment pos;
  pos.type = gfx::Attachment_Type::Color;
  pos.i = 0;

  gfx::Attachment normal;
  normal.type = gfx::Attachment_Type::Color;
  normal.i = 1;

  gfx::Attachment color;
  color.type = gfx::Attachment_Type::Color;
  color.i = 2;

  gfx::Attachment depth;
  depth.type = gfx::Attachment_Type::Depth_Stencil;
  depth.i = 0;

  fb_out.attachments.push_back(pos);
  fb_out.attachments.push_back(normal);
  fb_out.attachments.push_back(color);
  fb_out.attachments.push_back(depth);

  gfx::Deferred_Shading deferred(driver);
  deferred.init(Vec<int>{x, y}, fb_out);

  tinygltf::Scene deferred_gltf;
  if(!load_gltf_file(deferred_gltf, "../assets/gltf/library-pre.gltf"))
  {
    log_e("Failed to load deferred gltf");
    return EXIT_FAILURE;
  }

  std::string other_filename(argv[1]);

  tinygltf::Scene other_scene;
  if(!load_gltf_file(other_scene, other_filename))
  {
    log_e("Failed to load other gltf");
    return EXIT_FAILURE;
  }

  gfx::Asset asset = gfx::load_asset(driver, deferred_gltf);
  gfx::append_to_asset(driver, asset, other_scene);

  auto cam = gfx::make_fps_camera(driver.window_extents());
  cam.perspective.fov = glm::radians(75.0f);
  cam.fp.pos.y = 1.1f;
  cam.fp.pos.x = 1.1f;

  SDL_SetRelativeMouseMode(SDL_TRUE);

  driver.face_culling(true);

  gfx::Transformed_Light light0;
  light0.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.5f, 0.0f));

  light0.light.type = gfx::Light_Type::Spot;
  light0.light.color = glm::vec3(1.0f, 1.0f, 1.0f);

  light0.light.intensity = 1.0f;
  light0.light.distance = 1.0f;

  light0.light.constant_attenuation = 0.0f;
  light0.light.linear_attenuation = 1.0f;
  light0.light.quadratic_attenuation = 0.0f;

  light0.light.fall_off_angle = REDC_PI / 2.0f;
  light0.light.fall_off_exponent = 1.0f;

  gfx::Rendering_State render_state;
  render_state.driver = &driver;

  bool running = true;
  while(running)
  {
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
      switch(event.type)
      {
      case SDL_QUIT:
        running = false;
        break;
      case SDL_KEYDOWN:
        if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) running = false;
        if(event.key.keysym.scancode == SDL_SCANCODE_W) cam.fp.pos.y += .1f;
        if(event.key.keysym.scancode == SDL_SCANCODE_S) cam.fp.pos.y -= .1f;
        if(event.key.keysym.scancode == SDL_SCANCODE_A) cam.fp.pos.x -= .1f;
        if(event.key.keysym.scancode == SDL_SCANCODE_D) cam.fp.pos.x += .1f;
        if(event.key.keysym.scancode == SDL_SCANCODE_Q) cam.fp.pos.z -= .1f;
        if(event.key.keysym.scancode == SDL_SCANCODE_E) cam.fp.pos.z += .1f;
        break;
      case SDL_MOUSEMOTION:
        cam.fp.yaw += .001f * event.motion.xrel;
        cam.fp.pitch += .001f * event.motion.yrel;
        break;
      }
    }

    // Render envmap
    driver.clear();
    driver.set_blend_policy(gfx::Blend_Policy::Transparency);
    envmap.render(driver, cam);

    render_state.cur_technique_i = -1;
    render_state.cur_material_i = -1;
    render_asset(asset, cam, render_state);

    SDL_GL_SwapWindow(sdl_window);
  }

  return 0;
}

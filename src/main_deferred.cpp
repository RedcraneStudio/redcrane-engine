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
  if(!load_gltf_file(deferred_gltf, "../assets/gltf/deferred.gltf"))
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

  std::vector<gfx::Light> lights;

  gfx::Light light0;
  light0.pos = glm::vec3(0.0f, 1.5f, 0.0f);
  light0.power = 1.0f;
  light0.diffuse_color = glm::vec3(1.0f, 0.0f, 0.0f);
  light0.specular_color = glm::vec3(1.0f, 0.5f, 0.5f);
  lights.push_back(light0);

  gfx::Light light1;
  light1.pos = glm::vec3(1.5f, 1.5f, 0.0f);
  light1.power = 0.75f;
  light1.diffuse_color = glm::vec3(0.0f, 0.0f, 1.0f);
  light1.specular_color = glm::vec3(0.5f, 0.5f, 1.0f);
  lights.push_back(light1);

  gfx::Light light2;
  light2.pos = glm::vec3(0.0f, 1.5f, 1.5f);
  light2.power = 0.75f;
  light2.diffuse_color = glm::vec3(1.0f, 1.0f, 0.0f);
  light2.specular_color = glm::vec3(1.0f, 1.0f, 0.5f);
  lights.push_back(light2);

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

    deferred.use();

    driver.clear();
    gfx::Rendering_State state;
    render_asset(asset, cam, state);

    deferred.finish();

    driver.clear();
    driver.set_blend_policy(gfx::Blend_Policy::Transparency);
    envmap.render(driver, cam);
    deferred.render(cam, lights);

    SDL_GL_SwapWindow(sdl_window);
  }

  return 0;
}

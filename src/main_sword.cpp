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
#include "gfx/deferred.h"
#include "effects/envmap.h"

#include "rapidjson/document.h"
#include "glm/gtc/matrix_transform.hpp"

#include <boost/filesystem.hpp>

#include <chrono>
#include <vector>

template <class T>
double time_since(T before) noexcept
{
  T now = std::chrono::high_resolution_clock::now();
  using sec_t = std::chrono::duration<double, std::chrono::seconds::period>;
  return sec_t(now - before).count();
}

struct Attachment
{
  std::string name;
  redc::Asset asset;
};

struct Blade : public Attachment
{
};

struct Pommel : public Attachment
{

};

struct Guard : public Attachment
{

};

struct Hilt : public Attachment
{

};

struct Attachments
{
  std::vector<Blade>  blades;
  std::vector<Pommel> pommels;
  std::vector<Guard>  guards;
  std::vector<Hilt>   hilts;
};

using Attachment_Ref = std::size_t;
struct Sword_Configuration
{
  Attachment_Ref blade;
  Attachment_Ref pommel;
  Attachment_Ref guard;
  Attachment_Ref hilt;
};

template <class T>
T load_attachment_part(rapidjson::Value::Member const& mem,
                       boost::filesystem::path const& rel_path)
{
  T t;
  t.name = mem.name.GetString();

  // Find the asset name.
  std::string asset_name = mem.value["asset"].GetString();

  // Open relative to relative path
  boost::filesystem::path asset_path(
    mem.value["asset"].GetString()
    );

  asset_path = rel_path / asset_path;

  return t;
}

template <class T>
void load_attachment_set(rapidjson::Value const& val, std::vector<T>& vec,
                         boost::filesystem::path const& rel_path)
{
  rapidjson::Value::MemberIterator it = val.MemberBegin();
  rapidjson::Value::MemberIterator end = val.MemberEnd();
  for(; it != end; ++it)
  {
    T t = load_attachment_part<T>(*it, rel_path);
    vec.push_back(blade);
  }
}

Attachments load_attachments(rapidjson::Value const& doc,
                             boost::filesystem::path const& rel_path)
{
  Attachments ret;

  if_has_member(doc, "blades",  load_attachment_set, ret.blades,  rel_path);
  if_has_member(doc, "pommels", load_attachment_set, ret.pommels, rel_path);
  if_has_member(doc, "guards",  load_attachment_set, ret.guards,  rel_path);
  if_has_member(doc, "hilts",   load_attachment_set, ret.hilts,   rel_path);

  return ret;
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
          redc::init_sdl("Sword Test", {1000,1000}, false, false);

  auto sdl_window = sdl_init_raii_lock.window;

  int x, y;
  SDL_GetWindowSize(sdl_window, &x, &y);
  gfx::gl::Driver driver(Vec<int>{x,y});
  driver.set_clear_color(colors::clear_black);

  effects::Envmap_Effect envmap;
  envmap.init(driver);

  // Set up deferred shader

  gfx::Output_Interface fb_out;

  Attachment pos;
  pos.type = Attachment_Type::Color;
  pos.i = 0;

  Attachment normal;
  normal.type = Attachment_Type::Color;
  normal.i = 1;

  Attachment color;
  color.type = Attachment_Type::Color;
  color.i = 2;

  Attachment depth;
  depth.type = Attachment_Type::Depth_Stencil;
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



  tinygltf::Scene sword_scene;
  if(!load_gltf_file(sword_scene, "../assets/gltf/sword.gltf"))
  {
    log_e("Failed to load sword gltf");
    return EXIT_FAILURE;
  }

  Asset asset = load_asset(deferred_gltf);
  append_to_asset(asset, sword_scene);

  auto cam = gfx::make_fps_camera(driver.window_extents());
  cam.perspective.fov = glm::radians(60.0f);
  cam.fp.pos.z = -2.0f;

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
        break;
      }
    }

    deferred.use();

    driver.clear();
    Rendering_State state;
    render_asset(asset, cam, state);

    deferred.finish();

    driver.clear();
    envmap.render(driver, cam);
    deferred.render(cam, lights);

    SDL_GL_SwapWindow(sdl_window);
  }

  return 0;
}

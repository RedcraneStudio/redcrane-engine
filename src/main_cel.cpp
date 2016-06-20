/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#include "assets/load_dir.h"
#include "gfx/gl/driver.h"
#include "gfx/camera.h"
#include "sdl_helper.h"
#include "common/log.h"
#include "use/mesh.h"
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

  // Go into asset dir
  auto share_path = assets::share_path();
  boost::filesystem::current_path(share_path);

  auto sdl_init_raii_lock =
          redc::init_sdl("Cel shading Test", {1000,1000}, false, false);

  auto sdl_window = sdl_init_raii_lock.window;

  int x, y;
  SDL_GetWindowSize(sdl_window, &x, &y);
  gfx::gl::Driver driver(Vec<int>{x,y});
  driver.set_clear_color(colors::white);

  auto def_shade = driver.make_shader_repr();

  auto cel_shade_path = share_path / "shader" / "cel";
  load_vertex_file(*def_shade, (cel_shade_path / "vs.glsl").native());
  load_fragment_file(*def_shade, (cel_shade_path / "fs.glsl").native());

  def_shade->link();

  using namespace gfx::tags;
  def_shade->set_var_tag(model_tag, "model");
  def_shade->set_var_tag(view_tag, "view");
  def_shade->set_var_tag(proj_tag, "proj");
  def_shade->set_var_tag(diffuse_tag, "dif");

  def_shade->set_color(diffuse_tag, colors::white);

  // Cache the positions of these variables
  def_shade->tag_var("light_pos");
  def_shade->tag_var("offset");

  def_shade->set_vec3("light_pos", glm::vec3(0.0f, 0.0f, 1.0f));

  driver.use_shader(*def_shade);

  auto cam = gfx::make_fps_camera(driver.window_extents());
  cam.perspective.fov = glm::radians(90.0f);
  gfx::use_camera(driver, cam);

  driver.face_culling(true);

  auto monkey = gfx::load_mesh(driver, {"obj/monkey.obj"}).chunk;

  auto begin = std::chrono::high_resolution_clock::now();

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

    driver.clear();

    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.5f));
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    model = glm::rotate(model, (float) time_since(begin), glm::vec3(0.0f, 1.0f, 0.0f));

    driver.active_shader()->set_mat4(model_tag, model);

    def_shade->set_float("offset", 0.f);
    def_shade->set_color(diffuse_tag, colors::white);
    driver.cull_side(gfx::Cull_Side::Back);
    gfx::render_chunk(monkey);

    // Render it again, with an offset along the normal in black
    def_shade->set_float("offset", .05f);
    def_shade->set_color(diffuse_tag, colors::black);
    driver.cull_side(gfx::Cull_Side::Front);
    gfx::render_chunk(monkey);

    SDL_GL_SwapWindow(sdl_window);
  }

  return 0;
}

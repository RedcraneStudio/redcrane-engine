/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <thread>
#include <future>

#include "common/log.h"

#include "gfx/gl/driver.h"
#include "gfx/camera.h"
#include "gfx/mesh_chunk.h"
#include "gfx/idriver_ui_adapter.h"
#include "gfx/support/load_wavefront.h"
#include "gfx/support/mesh_conversion.h"
#include "gfx/support/generate_aabb.h"
#include "gfx/support/write_data_to_mesh.h"
#include "gfx/support/texture_load.h"
#include "gfx/support/software_texture.h"
#include "gfx/support/unproject.h"

#include "ui/load.h"
#include "ui/freetype_renderer.h"
#include "ui/mouse_logic.h"
#include "ui/simple_controller.h"

#include "map/map.h"
#include "map/water.h"
#include "map/terrain.h"

#include "stratlib/player_state.h"

#include "glad/glad.h"
#include "glfw3.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "uv.h"

#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

glm::vec4 project_point(glm::vec4 pt,
                        glm::mat4 const& model,
                        glm::mat4 const& view,
                        glm::mat4 const& proj) noexcept
{
  pt = proj * view * model * pt;
  pt /= pt.w;
  return pt;
}

struct Glfw_User_Data
{
  game::gfx::IDriver& driver;
  game::gfx::Camera& cam;
  game::ui::Mouse_State mouse_state;
};

void scroll_callback(GLFWwindow* window, double, double deltay)
{
  auto ptr = glfwGetWindowUserPointer(window);
  auto& data = *((Glfw_User_Data*) ptr);

  auto mworld = game::gfx::unproject_screen(data.driver, data.cam,
                                            glm::mat4(1.0f),
                                            data.mouse_state.position);

  auto ray_to_cam = glm::normalize(mworld - data.cam.fp.pos);
  data.cam.fp.pos += ray_to_cam * (float) deltay;
}

game::ui::Mouse_State gen_mouse_state(GLFWwindow* w)
{
  game::ui::Mouse_State ret;

  ret.button_down =glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

  game::Vec<double> pos;
  glfwGetCursorPos(w, &pos.x, &pos.y);
  ret.position = game::vec_cast<int>(pos);

  return ret;
}

void log_gl_limits(game::Log_Severity s) noexcept
{
  GLint i = 0;
  glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &i);
  log(s, "GL_MAX_ELEMENTS_VERTICES: %", i);
}

struct Command_Options
{
};

Command_Options parse_command_line(int argc, char**)
{
  Command_Options opt;
  for(int i = 0; i < argc; ++i)
  {
    //auto option = argv[i];
  }
  return opt;
}

int main(int argc, char** argv)
{
  using namespace game;

  set_log_level(Log_Severity::Debug);

  uv_chdir("assets/");

  // Initialize logger.
  Scoped_Log_Init log_init_raii_lock{};

  // Parse command line arguments.
  auto options = parse_command_line(argc - 1, argv+1);

  // Init glfw.
  if(!glfwInit())
    return EXIT_FAILURE;

  auto window = glfwCreateWindow(1000, 1000, "Hello World", NULL, NULL);
  if(!window)
  {
    glfwTerminate();
    return EXIT_FAILURE;
  }

  // Init context + load gl functions.
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

  // Log glfw version.
  log_i("Initialized GLFW %", glfwGetVersionString());

  int maj = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
  int min = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
  int rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);

  // Log GL profile.
  log_i("OpenGL core profile %.%.%", maj, min, rev);

  {
  // Make an OpenGL driver.
  gfx::gl::Driver driver{Vec<int>{1000, 1000}};

  // Load our default shader.
  auto default_shader = driver.make_shader_repr();
  default_shader->load_vertex_part("shader/basic/v");
  default_shader->load_fragment_part("shader/basic/f");

  default_shader->set_projection_name("proj");
  default_shader->set_view_name("view");
  default_shader->set_model_name("model");
  default_shader->set_sampler_name("tex");
  default_shader->set_diffuse_name("dif");
  driver.use_shader(*default_shader);

  default_shader->set_sampler(0);

  // Load our textures.
  auto grass_tex = driver.make_texture_repr();
  load_png("tex/grass.png", *grass_tex);

  // Make an isometric camera.
  auto cam = gfx::make_isometric_camera();

  // Load the image
  Software_Texture terrain_image;
  load_png("map/default.png", terrain_image);

  // Convert it into a heightmap
  Maybe_Owned<Mesh> terrain = driver.make_mesh_repr();

  auto terrain_heightmap = make_heightmap_from_image(terrain_image);
  auto terrain_data =
    make_terrain_mesh(terrain_heightmap, {20, 20}, .001f, .01);

  auto terrain_model = glm::scale(glm::mat4(1.0f),glm::vec3(5.0f, 1.0f, 5.0f));

  gfx::allocate_mesh_buffers(terrain_data.mesh, *terrain);
  gfx::write_data_to_mesh(terrain_data.mesh, ref_mo(terrain));
  gfx::format_mesh_buffers(*terrain);
  terrain->set_primitive_type(Primitive_Type::Triangle);

  // Map + structures.
  Maybe_Owned<Mesh> structure_mesh = driver.make_mesh_repr();
  Map map({1000, 1000}); // <-- Map size for now
  strat::Player_State player_state{strat::Player_State_Type::Nothing};

  auto structures = load_structures("structure/structures.json",
                                    ref_mo(structure_mesh),
                                    driver);

  int fps = 0;
  int time = glfwGetTime();

  // Set up some pre-rendering state.
  driver.clear_color_value(Color{0x55, 0x66, 0x77});
  driver.clear_depth_value(1.0);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  glEnable(GL_CULL_FACE);

  // Our quick hack to avoid splitting into multiple functions and dealing
  // either with global variables or like a bound struct or something.
  bool has_clicked_down = false;

  // Load our ui
  gfx::IDriver_UI_Adapter ui_adapter{driver};
  ui::Freetype_Renderer freetype_font;

  auto ui_load_params = ui::Load_Params{freetype_font, ui_adapter};
  auto hud = ui::load("ui/hud.json", ui_load_params);

  auto controller = ui::Simple_Controller{};

  hud->find_child_r("build_house")->add_click_listener([&](auto const& pt)
  {
    player_state.type = strat::Player_State_Type::Building;
    player_state.building.to_build = &structures[0];
  });
  hud->find_child_r("build_gvn_build")->add_click_listener([&](auto const& pt)
  {
    player_state.type = strat::Player_State_Type::Building;
    player_state.building.to_build = &structures[1];
  });

  hud->layout(driver.window_extents());

  controller.add_drag_listener([&](auto const& np, auto const& op)
  {
    auto oworld = gfx::unproject_screen(driver, cam, glm::mat4(1.0f), op);
    auto nworld = gfx::unproject_screen(driver, cam, glm::mat4(1.0f), np);

    cam.fp.pos += oworld - nworld;

    // This will give erroneous results if the user clicks on the part of the
    // screen with nothing drawn on it. So TODO: Put some checks in place so
    // we don't go messing with the camera zoom and whatnot.
  });
  controller.add_click_listener([&](auto const& mp)
  {
    if(player_state.type == strat::Player_State_Type::Building)
    {
      if(player_state.building.to_build == nullptr) return;
      auto world_p = gfx::unproject_screen(driver, cam, glm::mat4(1.0f), mp);

      // TODO: Sanitize this position a little better, maybe.
      pos_t map_pos{world_p.x, world_p.z};
      map.structures.emplace_back(*player_state.building.to_build, map_pos);
    }
  });

  Glfw_User_Data data{driver, cam};

  glfwSetWindowUserPointer(window, &data);
  glfwSetScrollCallback(window, scroll_callback);

  //water_obj.material->diffuse_color = Color{0xaa, 0xaa, 0xff};

  while(!glfwWindowShouldClose(window))
  {
    auto mouse_state = gen_mouse_state(window);
    data.mouse_state = mouse_state;

    ++fps;
    glfwPollEvents();

    // Clear the screen and render the terrain.
    driver.clear();
    use_camera(driver, cam);

    driver.bind_texture(*grass_tex, 0);

    default_shader->set_diffuse(colors::white);
    default_shader->set_model(terrain_model);

    // Render the terrain before we calculate the depth of the mouse position.
    terrain->draw_elements(0, terrain_data.mesh.elements.size());

    // Render any structures.
    // Maybe the mouse?

    // We only want to be able to pan the terrain for now. That's why we need
    // to do this before any structure rendering.
    auto mouse_world = gfx::unproject_screen(driver, cam, glm::mat4(1.0f),
                                             mouse_state.position);
    controller.step(hud, mouse_state);

    if(player_state.type == strat::Player_State_Type::Building)
    {
      auto const& st = *player_state.building.to_build;

      auto model = glm::translate(glm::mat4(1.0f), mouse_world);

      auto ray = ray_to_structure_bottom_center(st);
      model = glm::translate(model, -ray);

      driver.bind_texture(*st.texture(), 0);

      default_shader->set_model(model);
      gfx::render_chunk(st.mesh_chunk());
    }

    // Render all the other structures.
    for(auto const& st : map.structures)
    {
      // TODO: Find/store correct y somehow?
      glm::mat4 model = glm::translate(glm::mat4(1.0f),
                        glm::vec3(st.position.x, 0.0f, st.position.y));

      auto ray = ray_to_structure_bottom_center(st.structure());
      model = glm::translate(model, -ray);

      driver.bind_texture(*st.structure().texture(), 0);
      driver.active_shader()->set_model(model);

      gfx::render_chunk(st.structure().mesh_chunk());
    }

    {
      ui::Draw_Scoped_Lock scoped_draw_lock{ui_adapter};
      hud->render(ui_adapter);
    }
    glfwSwapBuffers(window);

    if(int(glfwGetTime()) != time)
    {
      time = glfwGetTime();
      log_d("fps: %", fps);
      fps = 0;
    }

    flush_log();
  }
  }
  glfwTerminate();
  return 0;
}

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
#include "common/value_map.h"

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
#include "gfx/support/render_normals.h"
#include "gfx/immediate_renderer.h"

#include "ui/load.h"
#include "ui/freetype_renderer.h"
#include "ui/mouse_logic.h"
#include "ui/simple_controller.h"
#include "ui/pie_menu.h"

#include "strat/map.h"
#include "strat/wall.h"
#include "strat/water.h"
#include "strat/terrain.h"
#include "strat/player_state.h"

#include "procedural/terrain.h"

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
  game::gfx::Camera& camera;
  game::ui::Element& root_hud;
  game::ui::Mouse_State& mouse_state;
};

void mouse_button_callback(GLFWwindow* window, int glfw_button, int action,int)
{
  using game::Vec; using game::ui::Mouse_State;

  auto user_ptr = *(Glfw_User_Data*) glfwGetWindowUserPointer(window);
  auto& mouse_state = user_ptr.mouse_state;

  bool down = (action == GLFW_PRESS);

  using namespace game::ui;
  Mouse_Button button;
  switch(glfw_button)
  {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
      button = Mouse_Button_Left;
      break;
    }
    case GLFW_MOUSE_BUTTON_RIGHT:
    {
      button = Mouse_Button_Right;
      break;
    }
    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
      button = Mouse_Button_Middle;
      break;
    }
  }

  if(down) mouse_state.buttons |= button;
  else mouse_state.buttons &= ~button;
}

void mouse_motion_callback(GLFWwindow* window, double x, double y)
{
  using game::ui::Mouse_State;

  auto user_ptr = *(Glfw_User_Data*) glfwGetWindowUserPointer(window);
  auto& mouse_state = user_ptr.mouse_state;

  mouse_state.position.x = x;
  mouse_state.position.y = y;
}

void scroll_callback(GLFWwindow* window, double, double deltay)
{
  using game::ui::Mouse_State;

  auto user_ptr = *(Glfw_User_Data*) glfwGetWindowUserPointer(window);
  auto& mouse_state = user_ptr.mouse_state;

  mouse_state.scroll_delta = deltay;
}

void log_gl_limits(game::Log_Severity s) noexcept
{
  GLint i = 0;
  glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &i);
  log(s, "GL_MAX_ELEMENTS_VERTICES: %", i);
}

void error_callback(int error, const char* description)
{
  game::log_d("GLFW Error: % (Code = %)", description, error);
}

void resize_callback(GLFWwindow* window, int width, int height)
{
  // Change OpenGL viewport
  glViewport(0, 0, width, height);

  auto user_ptr = *(Glfw_User_Data*) glfwGetWindowUserPointer(window);
  auto& idriver = user_ptr.driver;

  // Inform the driver of this change
  idriver.window_extents({width,height});

  // Change the camera aspect ratio
  user_ptr.camera.perspective.aspect = width / (float) height;

  // Perhaps relayout the hud here?
  user_ptr.root_hud.layout(game::Vec<int>{width, height});

}


int main(int argc, char** argv)
{
  using namespace game;

  set_log_level(Log_Severity::Debug);

  uv_chdir("assets/");

  // Initialize logger.
  Scoped_Log_Init log_init_raii_lock{};

  strat::Grid_Map grid_map;
  grid_map.allocate({128,128});

  std::random_device rnd{};
  auto prng = std::mt19937{rnd()};

  strat::Terrain_Params params{(int64_t) prng(), {64,64}, 88.0f,
                              strat::Landmass_Algorithm::Radial,
                              strat::Radial_Landmass{30.0f}};

  log_i("Seeding with %", params.seed);

  strat::terrain_v1_map(grid_map, params);
  strat::write_png_heightmap(grid_map, "../output.png");

  // Error callback
  glfwSetErrorCallback(error_callback);

  // Init glfw.
  if(!glfwInit())
    return EXIT_FAILURE;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

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

  // Set GLFW callbacks
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, mouse_motion_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetWindowSizeCallback(window, resize_callback);

  // UI Controller
  ui::Simple_Controller controller;

  {
  // Make an OpenGL driver.

  // Don't rely on any window resolution. Query it again.
  // After this, we'll query the driver for the real size.
  int window_width, window_height;
  glfwGetWindowSize(window, &window_width, &window_height);
  gfx::gl::Driver driver{Vec<int>{window_width, window_height}};

  // Load our default shader.
  auto default_shader = driver.make_shader_repr();
  default_shader->load_vertex_part("shader/basic/vs");
  default_shader->load_fragment_part("shader/basic/fs");

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

  // Load the image
  Software_Texture terrain_image;
  load_png("map/default.png", terrain_image);

  // Convert it into a heightmap
  Maybe_Owned<Mesh> terrain = driver.make_mesh_repr();

  auto terrain_heightmap = strat::make_heightmap_from_image(terrain_image);
  auto terrain_data =
    make_terrain_mesh(terrain_heightmap, {20, 20}, .001f, .01);

  auto terrain_model = glm::scale(glm::mat4(1.0f),glm::vec3(5.0f, 1.0f, 5.0f));

  gfx::allocate_mesh_buffers(terrain_data.mesh, *terrain);
  gfx::write_data_to_mesh(terrain_data.mesh, ref_mo(terrain));
  gfx::format_mesh_buffers(*terrain);
  terrain->set_primitive_type(Primitive_Type::Triangle);

  // Map + structures.
  Maybe_Owned<Mesh> structure_mesh = driver.make_mesh_repr();

  strat::Game_State game_state{&driver, gfx::make_isometric_camera(driver),
                               strat::Map{{1000, 1000}}}; // <-Map size for now

  strat::Player_State player_state{game_state};

  std::vector<Indexed_Mesh_Data> imd_vec;
  auto structures = strat::load_structures("structure/structures.json",
                                           ref_mo(structure_mesh),
                                           driver, &imd_vec);

  int fps = 0;
  int time = glfwGetTime();

  // Set up some pre-rendering state.
  driver.clear_color_value(Color{0x55, 0x66, 0x77});
  driver.clear_depth_value(1.0);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  glEnable(GL_CULL_FACE);

  // Load our ui
  gfx::IDriver_UI_Adapter ui_adapter{driver};
  ui::Freetype_Renderer freetype_font;

  auto ui_load_params = ui::Load_Params{freetype_font, ui_adapter};
  auto hud = ui::load("ui/hud.json", ui_load_params);

  hud->find_child_r("build_house")->on_step_mouse(
    ui::On_Release_Handler{[&](auto const& ms)
    {
      player_state.switch_state<strat::Building_State>(structures[0]);
    }});

  hud->find_child_r("build_gvn_build")->on_step_mouse(
    ui::On_Release_Handler{[&](auto const& ms)
    {
      player_state.switch_state<strat::Building_State>(structures[1]);
    }});
  hud->find_child_r("build_wall")->on_step_mouse(
    ui::On_Release_Handler{[&](auto const& ms)
    {
      strat::Wall_Type type{1, structures[2].aabb().width};
      player_state.switch_state<strat::Wall_Building_State>(type);
    }});

  hud->layout(driver.window_extents());

  auto cur_mouse = ui::Mouse_State{};

  // TODO: Put the camera somewhere else / Otherwise clean up the distinction
  // and usage of Glfw_User_Data, Game_State and Player_State.
  auto glfw_user_data = Glfw_User_Data{driver,game_state.cam, *hud, cur_mouse};
  glfwSetWindowUserPointer(window, &glfw_user_data);

  auto light_dir_pos = default_shader->get_location("light_dir");

  gfx::Immediate_Renderer ir{driver};
  std::size_t st_size = game_state.map.structures.size();
  while(!glfwWindowShouldClose(window))
  {
    ++fps;

    // Reset the scroll delta
    cur_mouse.scroll_delta = 0.0;

    if(st_size != game_state.map.structures.size()) ir.reset();

    // Update the mouse state.
    glfwPollEvents();

    // Clear the screen and render the terrain.
    driver.clear();
    use_camera(driver, game_state.cam);

    default_shader->set_vec3(light_dir_pos, glm::vec3(0.0f, 1.0f, 0.0f));

    driver.bind_texture(*grass_tex, 0);

    default_shader->set_diffuse(colors::white);
    default_shader->set_model(terrain_model);

    // Render the terrain before we calculate the depth of the mouse position.
    terrain->draw_elements(0, terrain_data.mesh.elements.size());

    // These functions are bound to get the depth from the framebuffer. Make
    // sure the depth value is only based on the terrain.
    {
      if(!controller.step(hud, cur_mouse))
      {
        player_state.step_mouse(cur_mouse);
      }

      // Handle zoom
      gfx::apply_zoom(game_state.cam, cur_mouse.scroll_delta,
                      cur_mouse.position, driver);
    }
    // Render any structures.
    // Maybe the mouse?

    // We only want to be able to pan the terrain for now. That's why we need
    // to do this before any structure rendering.
    //auto mouse_world = gfx::unproject_screen(driver, game_state.cam,
                                             //glm::mat4(1.0f),
                                             //mouse_state.position);

    //player_state.handle_mouse(Vec<float>{mouse_world.x, mouse_world.z});

    //if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS &&
       //player_state.type == strat::Player_State_Type::Nothing)
    //{
      //glfwSetWindowShouldClose(window, true);
    //}

    auto& pending_st = game_state.map.pending_structure;
    if(pending_st)
    {
      render_structure_instance(driver, pending_st.value());
    }

    // Render all the other structures.
    for(auto const& st : game_state.map.structures)
    {
      // TODO: Find/store correct y somehow?
      render_structure_instance(driver, st);

      if(st_size != game_state.map.structures.size())
      {
        if(&st.structure() == &structures[0])
        {
          gfx::render_normals(ir, imd_vec[0], st.model_matrix());
        }
        else if(&st.structure() == &structures[1])
        {
          gfx::render_normals(ir, imd_vec[1], st.model_matrix());
        }
      }
    }

    // Render walls
    if(game_state.map.pending_wall)
    {
      // Render the structure at the pending wall point.
      auto pos = game_state.map.pending_wall->pos;
      glm::mat4 model = glm::translate(glm::mat4(1.0f),
                                       glm::vec3(pos.x, 0.0f, pos.y));
      render_structure(driver, structures[2], model);
    }
    for(auto const& wall : game_state.map.walls)
    {
      render_wall(driver, wall, structures[2]);
    }

    ir.render(game_state.cam);

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

    st_size = game_state.map.structures.size();
  }
  }
  glfwTerminate();
  return 0;
}

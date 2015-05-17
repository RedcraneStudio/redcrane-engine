/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <thread>

#include "common/log.h"

#include "gfx/gl/driver.h"
#include "gfx/camera.h"
#include "gfx/object.h"
#include "gfx/scene_node.h"
#include "gfx/idriver_ui_adapter.h"

#include "common/software_texture.h"
#include "common/texture_load.h"
#include "common/software_mesh.h"
#include "common/mesh_load.h"
#include "common/mesh_write.h"

#include "ui/load.h"
#include "ui/freetype_renderer.h"
#include "ui/mouse_logic.h"
#include "ui/simple_controller.h"

#include "map/map.h"
#include "map/json_structure.h"
#include "glad/glad.h"
#include "glfw3.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "uv.h"

#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

void scroll_callback(GLFWwindow* window, double, double deltay)
{
  auto cam_ptr = glfwGetWindowUserPointer(window);
  auto& camera = *((game::gfx::Camera*) cam_ptr);

  camera.fp.pos += -deltay;
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

  // Make an isometric camera.
  auto cam = gfx::make_isometric_camera();

  auto terrain_obj = gfx::Object{};
  *terrain_obj.material = gfx::load_material("mat/terrain.json");

  // Build our terrain from a heightmap.
  {
    Software_Texture terrain_heightmap;
    load_png("map/default.png", terrain_heightmap);
    auto terrain = make_heightmap_from_image(terrain_heightmap);
    auto terrain_chunks =
      make_terrain_mesh(*terrain_obj.mesh, terrain, .01f, .01);
    write_obj("../terrain.obj", terrain_obj.mesh->mesh_data());
  }

  prepare_object(driver, terrain_obj);

  // Load our house structure
  auto house_struct = Json_Structure{"structure/house.json"};

  // This is code smell, right here. The fact that before, we were preparing
  // a temporary but it still worked because of the implementation of
  // Json_Structure and the object sharing mechanism.
  auto house_obj = house_struct.make_obj();
  prepare_object(driver, house_obj);

  std::vector<Structure_Instance> house_instances;
  Structure_Instance moveable_instance{house_struct, Orient::N};

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

  hud->find_child_r("build_button")->add_click_listener([](auto const& pt)
  {
    log_i("BUILD!");
  });

  hud->layout(driver.window_extents());

  controller.add_click_listener([&](auto const&)
  {
    // Commit the current position of our moveable instance.
    house_instances.push_back(moveable_instance);
  });
  controller.add_hover_listener([&](auto const& pt)
  {
    if(pt.x < 0 || pt.x > 1000.0 || pt.y < 0 || pt.y > 1000.0)
    {
      moveable_instance.obj.model_matrix = glm::mat4(1.0);
    }
    else
    {
      // Find our depth.
      GLfloat z;
      glReadPixels((float) pt.x, (float) 1000.0 - pt.y, 1, 1,
                   GL_DEPTH_COMPONENT, GL_FLOAT, &z);

      // Unproject our depth.
      auto val = glm::unProject(glm::vec3(pt.x, 1000.0 - pt.y, z),
                                camera_view_matrix(cam),
                                camera_proj_matrix(cam),
                                glm::vec4(0.0, 0.0, 1000.0, 1000.0));

      // We have our position, render a single house there.
      moveable_instance.obj.model_matrix = glm::translate(glm::mat4(1.0), val);
    }
    // Move the house by however much the structure *type* requires.
    moveable_instance.obj.model_matrix = moveable_instance.obj.model_matrix *
                                         house_struct.make_obj().model_matrix;
  });

  controller.add_drag_listener([&](auto const& np, auto const& op)
  {
    // pan
    auto movement = vec_cast<float>(np - op);
    movement /= -75;

    glm::vec4 move_vec(movement.x, 0.0f, movement.y, 0.0f);
    move_vec = glm::inverse(camera_view_matrix(cam)) * move_vec;

    cam.fp.pos.x += move_vec.x;
    cam.fp.pos.z += move_vec.z;
  });

  glfwSetWindowUserPointer(window, &cam);
  glfwSetScrollCallback(window, scroll_callback);

  while(!glfwWindowShouldClose(window))
  {
    ++fps;
    glfwPollEvents();

    // Clear the screen and render the terrain.
    driver.clear();
    use_camera(driver, cam);

    render_object(driver, terrain_obj);

    // Render the terrain before we calculate the depth of the mouse position.
    auto mouse_state = gen_mouse_state(window);
    controller.step(hud, mouse_state);

    // Render our movable instance of a house.
    render_object(driver, moveable_instance.obj);

    // Render all of our other house instances.
    for(auto const& instance : house_instances)
    {
      render_object(driver, instance.obj);
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

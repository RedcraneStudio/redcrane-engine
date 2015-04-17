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
#include "gfx/texture.h"
#include "gfx/mesh.h"

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
  gfx::gl::Driver driver{};

  // Make an isometric camera.
  auto cam = gfx::make_isometric_camera();
  driver.use_camera(cam);

  // Build our main mesh using our flat terrain.
  auto terrain_obj = gfx::Object{};
  terrain_obj.mesh.set_owned(
    make_terrain_mesh(make_flat_terrain(0, 25, 25), .01, 1));

  terrain_obj.material.set_owned(gfx::Material{});
  terrain_obj.material->diffuse_color = colors::white;
  terrain_obj.material->texture.set_owned(
    Texture::from_png_file("tex/grass.png"));

  terrain_obj.model_matrix = glm::translate(glm::mat4(1.0),
                                            glm::vec3(-12.5, 0.0, -12.5));

  prepare_object(driver, terrain_obj);

  // Load our house structure
  auto house_struct = Json_Structure{"structure/house.json"};
  prepare_object(driver, house_struct.make_obj());

  std::vector<Structure_Instance> house_instances;
  Structure_Instance moveable_instance{house_struct, Orient::N};

  int fps = 0;
  int time = glfwGetTime();

  // Set up some pre-rendering state.
  driver.clear_color_value(Color{0x55, 0x66, 0x77});
  driver.clear_depth_value(1.0);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  // Our quick hack to avoid splitting into multiple functions and dealing
  // either with global variables or like a bound struct or something.
  bool has_clicked_down = false;

  while(!glfwWindowShouldClose(window))
  {
    ++fps;

    // Clear the screen and render the terrain.
    driver.clear();
    render_object(driver, terrain_obj);

    glfwPollEvents();

    // Get our mouse coordinates.
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    if(x < 0 || x > 1000.0 || y < 0 || y > 1000.0)
    {
      moveable_instance.obj.model_matrix = glm::mat4(1.0);
    }
    else
    {
      GLfloat z;
      glReadPixels((float)x, (float) 1000.0 - y, 1, 1, GL_DEPTH_COMPONENT,
                   GL_FLOAT, &z);

      auto val = glm::unProject(glm::vec3(x, 1000.0 - y, z),
                                camera_view_matrix(cam),
                                camera_proj_matrix(cam),
                                glm::vec4(0.0, 0.0, 1000.0, 1000.0));

      // We have our position, render a single house there.
      moveable_instance.obj.model_matrix = glm::translate(glm::mat4(1.0), val);
    }

    // Move the house by however much the structure *type* requires.
    moveable_instance.obj.model_matrix =
      moveable_instance.obj.model_matrix *
      house_struct.make_obj().model_matrix;

    // Render our movable instance of a house.
    render_object(driver, moveable_instance.obj);

    // Render all of our other house instances.
    for(auto const& instance : house_instances)
    {
      render_object(driver, instance.obj);
    }

    glfwSwapBuffers(window);

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS &&
       !has_clicked_down)
    {
      has_clicked_down = true;

      // Commit the current position of our moveable instance.
      house_instances.push_back(moveable_instance);
    }
    else if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS)
    {
      has_clicked_down = false;
    }

    if(int(glfwGetTime()) != time)
    {
      time = glfwGetTime();
      log_d("fps: %", fps);
      fps = 0;
    }
  }
  }
  glfwTerminate();
  return 0;
}

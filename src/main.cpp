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
#include "gfx/object.h"
#include "gfx/scene.h"
#include "scene_node.h"

#include "texture.h"
#include "mesh.h"

#include "glad/glad.h"
#include "glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "uv.h"

#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

struct Command_Options
{
  bool test = false;
};

Command_Options parse_command_line(int argc, char** argv)
{
  Command_Options opt;
  for(int i = 0; i < argc; ++i)
  {
    auto option = argv[i];
    if(strcmp(option, "--test") == 0)
    {
      opt.test = true;
    }
  }
  return opt;
}

int main(int argc, char** argv)
{
  using namespace strat;

  set_log_level(Log_Severity::Debug);

  uv_chdir("assets/");

  // Initialize logger.
  Scoped_Log_Init log_init_raii_lock{};

  // Parse command line arguments.
  auto options = parse_command_line(argc - 1, argv+1);

  // Should be run the testing whatchamacallit?
  if(options.test)
  {
    int result = Catch::Session().run(1, argv);
    if(result)
    {
      return EXIT_FAILURE;
    }
  }

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

  // Make an OpenGL driver.
  auto driver = gfx::gl::Driver{};

  // Load the scene data for an isometric view
  auto scene_data = gfx::make_isometric_scene();

  // Load our root scene node from json
  auto scene_root = load_scene("scene/default.json", driver, scene_data);

  int fps = 0;
  int time = glfwGetTime();

  // Set up some pre-rendering state.
  driver.clear_color_value(Color{0x55, 0x66, 0x77});
  driver.clear_depth_value(1.0);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  float deg = -5;

  while(!glfwWindowShouldClose(window))
  {
    ++fps;

    // rotate 90 degrees ccw
    auto model = glm::mat4(1.0);
    model = glm::rotate(model, -3.14159f / 2.0f, glm::vec3(0, 1, 0));
    model = glm::rotate(model, deg, glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(5));
    deg += .001;

    scene_root.obj.model_matrix = model;

    // Clear the screen and render.
    driver.clear();
    scene_root.render();

    // Show it on screen.
    glfwSwapBuffers(window);
    glfwPollEvents();

    if(int(glfwGetTime()) != time)
    {
      time = glfwGetTime();
      log_d("fps: %", fps);
      fps = 0;
    }
  }

  glfwTerminate();
  return 0;
}

#if 0
int main()
{
  using namespace strat;

  auto driver = gfx::gl::Driver{"OpenGL 4.5"};
  auto scene_data = gfx::make_isometric_scene(factory, ...);

  auto scene_root = load_scene("scene/main.json");
  prepare_scene(factory, scene_root);

  while(window)
  {
    driver.clear();

    scene_root.render(scene_data);
    // Expanded view of ^
    {
      apply_model();
      material->use();
      mesh->render();

      for(child : children)
      {
        child.render(scene_data);
      }
    }

    gfx::swap_buffers();
  }
}
#endif

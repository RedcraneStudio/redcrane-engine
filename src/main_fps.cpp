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

#include "fps/camera_controller.h"

#include "glad/glad.h"
#include "glfw3.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "uv.h"

#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

#define PI 3.141592653589793238463

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

  // Hide the mouse and capture it
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  {
    // Make an OpenGL driver.
    gfx::gl::Driver driver{};

    // Make an fps camera.
    auto cam = gfx::make_fps_camera();
    cam.fp.pos = glm::vec3(0.0f, 0.0f, 15.0f);

    auto cam_controller = fps::Camera_Controller{};
    cam_controller.camera(cam);

    cam_controller.set_yaw_limit(PI / 2);
    cam_controller.set_pitch_limit(PI / 2);

    auto house = gfx::load_object("obj/house.obj", "mat/house.json");
    prepare_object(driver, house);

    auto house_aabb = generate_aabb(*house.mesh);

    auto plane = gfx::load_object("obj/plane.obj", "mat/plane.json");
    prepare_object(driver, plane);
    plane.model_matrix = glm::translate(plane.model_matrix,
                                        glm::vec3(0.0f,house_aabb.min.y,0.0f));
    plane.model_matrix = glm::scale(plane.model_matrix,
                                    glm::vec3(5.0f, 1.0f, 7.0f));
    plane.model_matrix = glm::translate(plane.model_matrix,
                                        glm::vec3(-0.5f, 0.0f, -0.5f));

    int fps = 0;
    int time = glfwGetTime();

    // Set up some pre-rendering state.
    driver.clear_color_value(Color{0x55, 0x66, 0x77});
    driver.clear_depth_value(1.0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    double prev_x, prev_y;
    glfwPollEvents();
    glfwGetCursorPos(window, &prev_x, &prev_y);

    while(!glfwWindowShouldClose(window))
    {
      ++fps;
      glfwPollEvents();

      if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      {
        glfwSetWindowShouldClose(window, true);
      }

      double x, y;
      glfwGetCursorPos(window, &x, &y);
      cam_controller.apply_delta_pitch(y / 250.0 - prev_y / 250.0);
      cam_controller.apply_delta_yaw(x / 250.0 - prev_x / 250.0);
      prev_x = x, prev_y = y;

      driver.use_camera(cam);

      // Clear the screen
      driver.clear();

      render_object(driver, house);
      render_object(driver, plane);

      glfwSwapBuffers(window);

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

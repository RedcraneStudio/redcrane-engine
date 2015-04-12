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
  using namespace strat;

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
  auto terrain_mesh = make_terrain_mesh(make_flat_terrain(0, 5, 5), .01, 1);
  driver.prepare_mesh(terrain_mesh);

  auto mat = gfx::Material{};
  mat.diffuse_color = colors::white;
  mat.texture = Maybe_Owned<Texture>(Texture::from_png_file("tex/grass.png"));
  driver.prepare_material(mat);

  // Load our house structure
  auto house_struct = Json_Structure{"structure/house.json"};
  house_struct.prepare(driver);

  int fps = 0;
  int time = glfwGetTime();

  // Set up some pre-rendering state.
  driver.clear_color_value(Color{0x55, 0x66, 0x77});
  driver.clear_depth_value(1.0);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  while(!glfwWindowShouldClose(window))
  {
    ++fps;

    // Clear the screen and render.
    driver.clear();
    driver.bind_material(mat);

    auto model = glm::mat4(1.0f);
    driver.set_model(model);
    driver.render_mesh(terrain_mesh);

    // Show it on screen.
    glfwPollEvents();

    double x, y;
    glfwGetCursorPos(window, &x, &y);

    if(x < 0 || x > 1000.0 || y < 0 || y > 1000.0)
    {
      house_struct.set_model(glm::mat4(1.0));
    }
    else
    {
      GLfloat z;
      glReadPixels((float)x, (float) 1000.0 - y, 1, 1, GL_DEPTH_COMPONENT,
                   GL_FLOAT, &z);

      auto val = glm::unProject(glm::vec3(x, 1000.0 - y, z),
                                camera_view_matrix(cam) * model,
                                camera_proj_matrix(cam),
                                glm::vec4(0.0, 0.0, 1000.0, 1000.0));

      // We have our position, render a small cube there.
      house_struct.set_model(glm::translate(glm::mat4(1.0), val));
    }
    house_struct.render(driver);

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

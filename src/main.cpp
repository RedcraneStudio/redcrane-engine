/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <thread>

#include "common/log.h"

#include "gfx/gl/program.h"
#include "gfx/gl/driver.h"
#include "gfx/gl/diffuse_material.h"
#include "gfx/gl/program_cache.h"

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
  using namespace survive;

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

  // Load a shader program.
  //auto shader_program = gfx::gl::Program::from_files("shader/diffuse/vertex",
                                                 //"shader/diffuse/fragment");

  //auto material = std::make_unique<gfx::gl::Diffuse_Material>();
  //auto shader = gfx::gl::load_program("shader/diffuse/decl.json");
  //auto sampler_loc = shader->get_uniform_location("tex");
  //auto model_loc = shader->get_uniform_location("model");
  //auto proj_loc = shader->get_uniform_location("proj");
  //auto view_loc = shader->get_uniform_location("view");

  //auto tex =
        //driver.prepare_texture(Texture::from_png_file("tex/cracked_soil.png"));
  //material->diffuse_color(Color{0xff, 0x00, 0x00});
  //material->texture(std::move(tex));
  //tex->bind(0);
  //glUniform1i(sampler_loc, 0);

  // Load the scene data for an isometric view
  auto scene_data = gfx::make_isometric_scene();
  //scene_data.register_observer(*material);

  glm::mat4 view(1.0);
  view = glm::lookAt(glm::vec3(0, 0, 15), glm::vec3(0, 0, 0),
                     glm::vec3(0, 1, 0));
  //scene_data.view_matrix(view);
  glm::mat4 perspective(1.0);
  perspective = glm::perspective(45., 1., .01, 30.);
  //scene_data.projection_matrix(perspective);

  //material->set_view(view);
  //material->set_projection(perspective);

  auto scene = load_scene("scene/default.json", driver);
  scene_data.register_observer(*scene.obj.material);
  //prepare_scene(driver);

  // Prepare a mesh for rendering.
  //auto mesh = driver.prepare_mesh(Mesh::from_file("obj/plane.obj"));

  int fps = 0;
  int time = glfwGetTime();

  // Set up some pre-rendering state.
  glClearColor(0.509, .694, .737, 1.0);
  glClearDepth(1);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  // Swap the back buffer to front buffer, first?
  glfwSwapBuffers(window);


  // Use our shader.


  float deg = -5;

  //shader->use();

  //material->use(model);
  //glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &perspective[0][0]);
  //glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view[0][0]);

  //glUseProgram(0);

  bool done = false;
  while(!glfwWindowShouldClose(window) && !done)
  {
    //done = true;
    ++fps;

    //shader->use();
    // Create our matrix.

    //glm::mat4 view = glm::lookAt(glm::vec3(0, 10, 0), glm::vec3(0, 0, 0),
                                 //glm::vec3(0, 0, 1));
    //glm::mat4 perspective = glm::frustum(-1, 1, -1, 1, -1, 1);
    //glm::mat4 perspective = glm::ortho(-1, 1, -1, 1, -1, 1);

    // rotate 90 degrees ccw
    auto model = glm::mat4(1.0);
    //model = glm::translate(model, glm::vec3(0, cam_height, 0));
    model = glm::rotate(model, -3.14159f / 2.0f, glm::vec3(0, 1, 0));
    model = glm::rotate(model, deg, glm::vec3(0, 1, 0));
    //model = glm::translate(model, glm::vec3(0, 0, 0));
    //model = glm::translate(model, glm::vec3(0, 0, 7));
    model = glm::scale(model, glm::vec3(5));
    //glUniformMatrix4fv(model_loc, 1, GL_FALSE, &model[0][0]);

    deg += .001;
    scene.model = model;

    // Set the matrix in the program.
    // TODO this seems bad, change this.

    // Clear the screen and render.
    driver.clear();
    scene.render();

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
  using namespace survive;

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

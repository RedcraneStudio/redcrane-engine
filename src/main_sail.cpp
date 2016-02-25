/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <thread>
#include <chrono>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "common/maybe_owned.hpp"
#include "common/log.h"

#include "gfx/gl/driver.h"
#include "gfx/camera.h"
#include "gfx/mesh_chunk.h"
#include "gfx/mesh_data.h"
#include "gfx/support/load_wavefront.h"
#include "gfx/support/mesh_conversion.h"
#include "gfx/support/generate_aabb.h"
#include "gfx/support/write_data_to_mesh.h"
#include "gfx/support/texture_load.h"
#include "gfx/support/format.h"
#include "gfx/support/allocate.h"
#include "gfx/support/json.h"
#include "fps/load_scene.h"

#include "collisionlib/triangle_conversion.h"
#include "collisionlib/triangle.h"
#include "collisionlib/motion.h"

#include "boatlib/boat.h"
#include "sail/game.h"

#include "use/mesh.h"
#include "use/texture.h"

#include "gfx/support/proj_grid.h"
#include "terrain/chunks.h"

#include "common/json.h"

#include "fps/camera_controller.h"

#include "glad/glad.h"
#include "glfw3.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

#include "uv.h"

#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

#include <boost/program_options.hpp>

#define PI 3.141592653589793238463

void error_callback(int error, const char* description)
{
  redc::log_d("GLFW Error: % (Code = %)", description, error);
}

struct Glfw_User_Data
{
  redc::gfx::IDriver& driver;
  redc::gfx::Camera& camera;
  bool forward_pressed = false;
  bool left_pressed = false;
  bool back_pressed = false;
  bool right_pressed = false;
  bool should_spawn_projectile = false;
  bool update_cam = true;
};
void mouse_button_callback(GLFWwindow*, int, int, int)
{
}
void mouse_motion_callback(GLFWwindow*, double, double)
{
}
void key_callback(GLFWwindow* window, int key, int, int action, int)
{
  using namespace redc;

  auto user_ptr = (Glfw_User_Data*) glfwGetWindowUserPointer(window);

  if(action == GLFW_PRESS)
  {
    switch(key)
    {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, true);
        break;
      case GLFW_KEY_W:
        user_ptr->forward_pressed = true;
        break;
      case GLFW_KEY_A:
        user_ptr->left_pressed = true;
        break;
      case GLFW_KEY_S:
        user_ptr->back_pressed = true;
        break;
      case GLFW_KEY_D:
        user_ptr->right_pressed = true;
        break;
      case GLFW_KEY_SPACE:
        user_ptr->should_spawn_projectile = true;
        break;
      case GLFW_KEY_C:
        user_ptr->update_cam = false;
        break;
    }
  }
  else if(action == GLFW_RELEASE)
  {
    switch(key)
    {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, true);
        break;
      case GLFW_KEY_W:
        user_ptr->forward_pressed = false;
        break;
      case GLFW_KEY_A:
        user_ptr->left_pressed = false;
        break;
      case GLFW_KEY_S:
        user_ptr->back_pressed = false;
        break;
      case GLFW_KEY_D:
        user_ptr->right_pressed = false;
        break;
      case GLFW_KEY_C:
        user_ptr->update_cam = true;
        break;
    }
  }
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
}

int main(int argc, char** argv)
{
  using namespace redc;

  // Parse command line options
  auto vm = sail::parse_command_options(argc, argv);

  // House-keeping, etc
  if(vm.count("help"))
  {
    std::cerr << sail::command_options_desc() << std::endl;
    return EXIT_SUCCESS;
  }

  // Initialize logger.
  Scoped_Log_Init log_init_raii_lock{};

  set_out_log_level((Log_Severity) vm["out-log-level"].as<unsigned int>());
  set_file_log_level((Log_Severity) vm["file-log-level"].as<unsigned int>());

  if(vm.count("log-file"))
  {
    set_log_file(vm["log-file"].as<std::string>());
  }

  int ret_code = EXIT_SUCCESS;

  // Figure out the server situation, then pass control to the corresponding
  // function of that mode.
  auto server_mode = sail::pick_server_mode(vm);
  if(server_mode == sail::Server_Mode::Bad)
  {
    log_e("Pick either dedicated-server, local-server, or connect");
    ret_code = EXIT_FAILURE;
  }
  else if(server_mode == sail::Server_Mode::Dedicated)
  {
    ret_code = sail::start_dedicated(vm);
  }
  else if(server_mode == sail::Server_Mode::Connect)
  {
    ret_code = sail::start_connect(vm);
  }
  else
  {
    ret_code = sail::start_game(vm);
  }

  flush_log_full();

  return ret_code;
}

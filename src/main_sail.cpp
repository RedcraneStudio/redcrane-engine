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

#include "common/json.h"

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

void error_callback(int error, const char* description)
{
  game::log_d("GLFW Error: % (Code = %)", description, error);
}

struct Glfw_User_Data
{
  game::gfx::IDriver& driver;
  game::gfx::Camera& camera;
  bool forward_pressed = false;
  bool left_pressed = false;
  bool back_pressed = false;
  bool right_pressed = false;
};
void mouse_button_callback(GLFWwindow*, int, int, int)
{
}
void mouse_motion_callback(GLFWwindow*, double, double)
{
}
void key_callback(GLFWwindow* window, int key, int, int action, int)
{
  using namespace game;

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
  using namespace game;

  set_log_level(Log_Severity::Debug);

  uv_chdir("assets/");

  // Initialize logger.
  Scoped_Log_Init log_init_raii_lock{};

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

  // Disable vsync
  glfwSwapInterval(0);

  // Log glfw version.
  log_i("Initialized GLFW %", glfwGetVersionString());

  int maj = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
  int min = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
  int rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);

  // Log GL profile.
  log_i("OpenGL core profile %.%.%", maj, min, rev);

  // Hide the mouse and capture it
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glfwSetWindowSizeCallback(window, resize_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, mouse_motion_callback);
  glfwSetKeyCallback(window, key_callback);

  {
    // Make an OpenGL driver.
    int window_width, window_height;
    glfwGetWindowSize(window, &window_width, &window_height);
    gfx::gl::Driver driver{Vec<int>{window_width, window_height}};

    auto shader = driver.make_shader_repr();
    shader->load_vertex_part("shader/basic/vs.glsl");
    shader->load_fragment_part("shader/basic/fs.glsl");

    // We need to get rid of dependency on these in the future.
    shader->set_diffuse_name("dif");
    shader->set_projection_name("proj");
    shader->set_view_name("view");
    shader->set_model_name("model");
    shader->set_sampler_name("tex");

    auto light_pos_loc = shader->get_location("light_pos");

    driver.use_shader(*shader);

    shader->set_vec3(light_pos_loc, glm::vec3(0.0f, 1.0f, 0.0f));
    shader->set_sampler(0);

    // Get all our textures
    auto brick = driver.make_texture_repr();
    load_png("tex/cracked_soil.png", *brick);
    auto grass = driver.make_texture_repr();
    load_png("tex/grass.png", *grass);

    auto terrain_tex = driver.make_texture_repr();
    load_png("tex/topdown_terrain.png", *terrain_tex);

    auto boat_mesh = Maybe_Owned<Mesh>{driver.make_mesh_repr()};
    auto boat_data =
      gfx::to_indexed_mesh_data(gfx::load_wavefront("obj/boat.obj"));
    gfx::allocate_standard_mesh_buffers(boat_data.vertices.size(),
                                        boat_data.elements.size(),
                                        *boat_mesh,
                                        Usage_Hint::Draw,
                                        Upload_Hint::Static);
    gfx::format_standard_mesh_buffers(*boat_mesh);
    auto boat_chunk = gfx::write_data_to_mesh(boat_data, ref_mo(boat_mesh),
                                              0, 0);

    auto boat_motion = collis::Motion{};
    boat_motion.mass = 340; // 340 N is about the weight of a small sailboat.
    boat_motion.angular.radius = 1;
    glm::mat4 boat_model{1.0f};

    // Make an fps camera.
    auto cam = gfx::make_fps_camera(driver);
    cam.fp.pos = glm::vec3(0.0f, 1.0f, 0.0f);

    auto cam_controller = fps::Camera_Controller{};
    cam_controller.camera(cam);

    cam_controller.set_pitch_limit(PI / 2);

    auto glfw_user_data = Glfw_User_Data{driver, cam};
    glfwSetWindowUserPointer(window, &glfw_user_data);

    int fps = 0;
    int time = glfwGetTime();

    // Set up some pre-rendering state.
    driver.clear_color_value(Color{0x55, 0x66, 0x77});
    driver.clear_depth_value(1.0);

    driver.depth_test(true);
    glDepthFunc(GL_LEQUAL);

    double prev_x, prev_y;
    glfwPollEvents();
    glfwGetCursorPos(window, &prev_x, &prev_y);

    shader->set_diffuse(colors::white);
    shader->set_model(glm::mat4(1.0f));

    float prev_time = glfwGetTime();
    while(!glfwWindowShouldClose(window))
    {
      ++fps;

      // Clear the forces on the boat
      collis::reset_force(boat_motion.displacement);
      collis::reset_torque(boat_motion.angular);

      // Handle events (likely will add a force or two.
      glfwPollEvents();

      // This only works if the torque is only vertical, which in our case it
      // is. That is, we are assuming that the axis of rotation is up or down.
      glm::vec3 boat_dir = glm::vec3(0.0f, 0.0f, 1.0f);
      // TODO: Find a better way to handle nan
      if(boat_motion.angular.displacement.x != 0)
      {
        boat_dir = glm::rotate(boat_dir, glm::length(boat_motion.angular.displacement),
                               glm::normalize(boat_motion.angular.displacement));
      }
#if 0
      glm::vec3 boat_dir = glm::vec3(-glm::sin(boat_angle), 0.0f,
                                     glm::cos(boat_angle));
#endif

      if(glfw_user_data.forward_pressed == true)
      {
        collis::apply_force(boat_motion.displacement, boat_dir * -100.0f);
      }
      if(glfw_user_data.back_pressed == true)
      {
        collis::apply_force(boat_motion.displacement, boat_dir * 100.0f);
      }
      if(glfw_user_data.left_pressed == true)
      {
        collis::apply_torque(boat_motion.angular,
                             glm::vec3(0.0f, 0.0f, 1.0f),// Apply it to the front
                             glm::vec3(50.0f, 0.0f, 0.0f)); // of the boat
      }
      if(glfw_user_data.right_pressed == true)
      {
        collis::apply_torque(boat_motion.angular,
                             glm::vec3(0.0f, 0.0f, 1.0f),
                             glm::vec3(-50.0f, 0.0f, 0.0f));
      }

      double x, y;
      glfwGetCursorPos(window, &x, &y);
      cam_controller.apply_delta_pitch(y / 250.0 - prev_y / 250.0);
      cam_controller.apply_delta_yaw(x / 250.0 - prev_x / 250.0);
      prev_x = x, prev_y = y;

      // Take the velocity of the boat and use it to calculate a drag force.
      // Just for shits n' giggles lets simply set a maximum velocity like this
      auto p = 971.8f; // Density of water at 80 deg C I think. kg / m^3
      auto v = glm::length(boat_motion.displacement.velocity); // m/s
      auto C_d = 0.10f; // I think a boat should be pretty low. No unit?
      auto A = 5.0f; // This is almost a random guess.
      glm::vec3 f_d = (.5f * p * v * v * C_d * A) *
        glm::normalize(-boat_motion.displacement.velocity);

      // Apply the drag force.
      if(!std::isnan(f_d.x) && !std::isnan(f_d.y) && !std::isnan(f_d.z))
      {
        collis::apply_force(boat_motion.displacement, f_d);
        log_i("%", glm::length(f_d));
      }

      // Solve the motion
      auto new_time = glfwGetTime();
      solve_motion(new_time - prev_time, boat_motion);
      prev_time = new_time;

      // Calculate a model
      boat_model = glm::mat4(1.0f);
      boat_model = glm::translate(boat_model,
        boat_motion.displacement.displacement);
      if(glm::length(boat_motion.angular.displacement) > .001)
      {
        boat_model = glm::rotate(boat_model,
          glm::length(boat_motion.angular.displacement),
          glm::normalize(boat_motion.angular.displacement));
      }

      use_camera(driver, cam);

      // Clear the screen
      driver.clear();

      // Set the boat model matrix
      shader->set_model(boat_model);
      // Render the boat
      gfx::render_chunk(boat_chunk);

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

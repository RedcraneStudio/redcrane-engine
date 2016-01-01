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
  redc::log_d("GLFW Error: % (Code = %)", description, error);
}

struct Glfw_User_Data
{
  redc::gfx::IDriver& driver;
  redc::gfx::Camera& camera;
};
void mouse_button_callback(GLFWwindow* window, int glfw_button, int action,int)
{
}
void mouse_motion_callback(GLFWwindow* window, double x, double y)
{
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

  set_out_log_level(Log_Severity::Debug);

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
    auto brick_tex_data = load_png_data("tex/cracked_soil.png");
    blit_image(*brick, brick_tex_data);
    auto grass = driver.make_texture_repr();
    auto grass_tex_data = load_png_data("tex/grass.png");
    blit_image(*grass, grass_tex_data);

    auto terrain_tex = driver.make_texture_repr();
    auto terrain_tex_data = load_png_data("tex/topdown_terrain.png");
    blit_image(*terrain_tex, terrain_tex_data);

    auto scene_mesh = driver.make_mesh_repr();
    auto scene = fps::load_scene("scene/fps.json", std::move(scene_mesh));

    // Make an fps camera.
    auto cam = gfx::make_fps_camera(driver);
    cam.fp.pos = scene.player_pos;

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

    while(!glfwWindowShouldClose(window))
    {
      ++fps;

      glfwPollEvents();

      if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      {
        glfwSetWindowShouldClose(window, true);
      }

      glm::vec4 delta_movement;
      if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      {
        delta_movement.z -= 1.0f;
      }
      if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      {
        delta_movement.x -= 1.0f;
      }
      if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      {
        delta_movement.z += 1.0f;
      }
      if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      {
        delta_movement.x += 1.0f;
      }

      // Temporary replacement for shift, I just didn't feel like looking it up
      if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
      {
        delta_movement *= .0005;
      }
      else
      {
        delta_movement *= .005;
      }

      delta_movement = delta_movement *
        glm::rotate(glm::mat4(1.0f), cam.fp.yaw, glm::vec3(0.0f, 1.0f, 0.0f));

      double x, y;
      glfwGetCursorPos(window, &x, &y);
      cam_controller.apply_delta_pitch(y / 250.0 - prev_y / 250.0);
      cam_controller.apply_delta_yaw(x / 250.0 - prev_x / 250.0);
      prev_x = x, prev_y = y;

      cam.fp.pos.x += delta_movement.x;
      cam.fp.pos.z += delta_movement.z;

      // Find the height at cam.fp.pos.
      for(auto const& triangle : scene.collision_triangles)
      {
        while(collis::espace_is_intersecting(triangle, cam.fp.pos))
        {
          cam.fp.pos.y += .01f;
        }
      }

      use_camera(driver, cam);

      // Clear the screen
      driver.clear();

      for(auto const& obj : scene.objects)
      {
        driver.active_shader()->set_model(obj.model);
        gfx::render_chunk(obj.mesh);
      }

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

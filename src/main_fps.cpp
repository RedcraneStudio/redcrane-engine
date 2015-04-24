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

#include <btBulletDynamicsCommon.h>

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

struct House_Kinematic_Motion : public btMotionState
{
  void getWorldTransform(btTransform &trans) const override;
  void setWorldTransform(btTransform const& trans) override;

  void move_left();
  void move_right();
private:
  btVector3 pos_;
};

void House_Kinematic_Motion::getWorldTransform(btTransform &trans) const
{
  trans.setOrigin(pos_);
}
void House_Kinematic_Motion::setWorldTransform(btTransform const& trans)
{
  pos_ = trans.getOrigin();
}

void House_Kinematic_Motion::move_left()
{
  pos_.setX(pos_.getX() - .01);
}
void House_Kinematic_Motion::move_right()
{
  pos_.setX(pos_.getX() + .01);
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

  // Initialize bullet

  btDbvtBroadphase bt_broadphase;

  btDefaultCollisionConfiguration bt_configuration;
  btCollisionDispatcher bt_collision_dispatcher(&bt_configuration);

  btSequentialImpulseConstraintSolver bt_constraint_solver;
  btDiscreteDynamicsWorld bt_world(&bt_collision_dispatcher,
                                   &bt_broadphase,
                                   &bt_constraint_solver,
                                   &bt_configuration);

  bt_world.setGravity(btVector3(0, -9.81, 0));

  btStaticPlaneShape bt_plane_shape(btVector3(0, 1, 0), -1);
  btDefaultMotionState bt_motion_state;
  btRigidBody bt_plane(0, &bt_motion_state, &bt_plane_shape);

  bt_world.addCollisionObject(&bt_plane);

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

    btBoxShape bt_house_shape(btVector3(house_aabb.width / 2.f,
                                        house_aabb.height / 2.f,
                                        house_aabb.depth / 2.f));
    House_Kinematic_Motion bt_house_motion_state;
    btRigidBody bt_house_rigidbody(100,&bt_house_motion_state,&bt_house_shape);
    bt_house_rigidbody.setCollisionFlags(
                       btCollisionObject::CF_KINEMATIC_OBJECT);

    btTransform house_transform(btMatrix3x3::getIdentity(),
                                btVector3(0, 0, 0));
    bt_house_motion_state.setWorldTransform(house_transform);

    bt_world.addRigidBody(&bt_house_rigidbody);

    auto plane = gfx::load_object("obj/plane.obj", "mat/plane.json");
    prepare_object(driver, plane);
    plane.model_matrix = glm::scale(plane.model_matrix,
                                    glm::vec3(5.0f, 1.0f, 7.0f));
    plane.model_matrix = glm::translate(plane.model_matrix,
                                        glm::vec3(0.0f,-1.0f,0.0f));
    plane.model_matrix = glm::translate(plane.model_matrix,
                                        glm::vec3(-0.5f, 0.0f, -0.5f));

    auto sphere = gfx::load_object("obj/sphere.obj", "mat/plane.json");
    prepare_object(driver, sphere);

    btDefaultMotionState default_motion;
    btCapsuleShape sphere_shape(1.0f, 1.0f);
    btRigidBody sphere_body(50, &default_motion, &sphere_shape);

    btTransform sphere_initial;
    sphere_initial.setOrigin(btVector3(0.0f, 5.0f, 0.0f));
    sphere_body.setWorldTransform(sphere_initial);

    bt_world.addRigidBody(&sphere_body);

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

    using time_point_t =
                   std::chrono::time_point<std::chrono::high_resolution_clock>;
    time_point_t before = time_point_t::clock::now();

    while(!glfwWindowShouldClose(window))
    {
      ++fps;

      if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
      {
        bt_house_motion_state.move_left();
      }
      if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
      {
        bt_house_motion_state.move_right();
      }

      auto now = time_point_t::clock::now();
      auto diff = now - before;
      before = now;

      using std::chrono::duration_cast;
      auto ms = duration_cast<std::chrono::milliseconds>(diff).count();

      bt_world.stepSimulation(ms / 1000.0f, 6);

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

      btTransform transform;
      bt_house_motion_state.getWorldTransform(transform);
      auto orig = transform.getOrigin();
      house.model_matrix = glm::translate(glm::mat4(1.0f),
                                          glm::vec3(orig.x(), orig.y(),
                                                    orig.z()));

      auto y_delta_for_now = house_aabb.min.y - house_aabb.height / 2;

      house.model_matrix = glm::translate(house.model_matrix,
                                          glm::vec3(0.f,
                                          -house_aabb.height / 2.f -
                                          house_aabb.min.y,0.f));

      render_object(driver, house);
      render_object(driver, plane);

      btTransform sphere_trans;
      default_motion.getWorldTransform(sphere_trans);
      sphere_trans.getOpenGLMatrix(&sphere.model_matrix[0][0]);

      render_object(driver, sphere);

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

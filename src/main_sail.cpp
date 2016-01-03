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

#include "sail/player_data.h"
#include "sail/boat.h"

#include "use/mesh.h"
#include "use/texture.h"

#include "water/grid.h"
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

  namespace po = boost::program_options;

  po::options_description general_opt("General");
  general_opt.add_options()
    ("help", "display help")
    ("out-log-level", po::value<unsigned int>()->default_value(2),
     "set minimum log level to stdout")
    ("log-file", po::value<std::string>(), "set log file")
    ("file-log-level", po::value<unsigned int>()->default_value(0),
     "set minimum log level to file")
  ;

  po::options_description boat_opt("Boat");
  boat_opt.add_options()
    ("hull", po::value<unsigned int>()->default_value(0), "set boat hull")
    ("sail", po::value<unsigned int>()->default_value(0), "set boat sail")
    ("rudder", po::value<unsigned int>()->default_value(0), "set boat rudder")
    ("gun", po::value<unsigned int>()->default_value(0), "set boat gun")
  ;

  po::options_description server_opt("Server");
  server_opt.add_options()
    ("port", po::value<uint16_t>()->default_value(28222), "set port number")
    ("max-peers", po::value<uint16_t>()->default_value(12),
     "set max number of connections")
    ("local-server", "start local server with a client gui")
    ("dedicated-server", "start a dedicated server without a client gui")
    ("advertise-server", "advertise the server to other clients")
    ("connect", po::value<std::string>(), "connect to a server")
  ;

  po::options_description desc("Allowed Options");

  desc.add(general_opt).add(boat_opt).add(server_opt);

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if(vm.count("help"))
  {
    std::cerr << desc << std::endl;
    return EXIT_SUCCESS;
  }

  // Initialize logger.
  Scoped_Log_Init log_init_raii_lock{};

  // Figure out proper log level
  set_out_log_level((Log_Severity) vm["out-log-level"].as<unsigned int>());
  set_file_log_level((Log_Severity) vm["file-log-level"].as<unsigned int>());

  if(vm.count("log-file"))
  {
    set_log_file(vm["log-file"].as<std::string>());
  }

  uv_chdir("assets/");

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

    // Make a cache
    gfx::Mesh_Cache mesh_cache(driver);

    // Figure out what hulls, sails, rudders, and guns we have available.
    Boat_Descs boat_descs = build_default_descs(mesh_cache);
    log_boat_descs(boat_descs);

    Boat_Config boat_config;

    boat_config.hull = &boat_descs.hull_descs[vm["hull"].as<unsigned int>()];
    boat_config.sail = &boat_descs.sail_descs[vm["sail"].as<unsigned int>()];
    boat_config.rudder = &boat_descs.rudder_descs[vm["rudder"].as<unsigned int>()];
    boat_config.gun = &boat_descs.gun_descs[vm["gun"].as<unsigned int>()];

    log_i("Chosen boat configuration: % (Hull), % (Sail), % (Rudder), "
          "% (Gun)", boat_config.hull->name, boat_config.sail->name,
          boat_config.rudder->name, boat_config.gun->name);

    Boat_Render_Config boat_render = build_boat_render_config(boat_config);

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

    glm::vec3 light_pos(0.0f, 5.0f, 0.0f);
    shader->set_vec3(light_pos_loc, light_pos);
    shader->set_sampler(0);

    // Load environment map
    auto envtex = gfx::load_cubemap(driver, "tex/envmap/front.png",
                                    "tex/envmap/back.png",
                                    "tex/envmap/right.png",
                                    "tex/envmap/left.png",
                                    "tex/envmap/up.png",
                                    "tex/envmap/down.png");

    std::vector<float> env_cube_data =
    {
      -1.0f, +1.0f, -1.0f,
      -1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, +1.0f, -1.0f,
      -1.0f, +1.0f, -1.0f,

      -1.0f, -1.0f,  1.0f,
      -1.0f, -1.0f, -1.0f,
      -1.0f, +1.0f, -1.0f,
      -1.0f, +1.0f, -1.0f,
      -1.0f, +1.0f,  1.0f,
      -1.0f, -1.0f,  1.0f,

       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f,  1.0f,
       1.0f, +1.0f,  1.0f,
       1.0f, +1.0f,  1.0f,
       1.0f, +1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,

      -1.0f, -1.0f,  1.0f,
      -1.0f, +1.0f,  1.0f,
       1.0f, +1.0f,  1.0f,
       1.0f, +1.0f,  1.0f,
       1.0f, -1.0f,  1.0f,
      -1.0f, -1.0f,  1.0f,

      -1.0f,  1.0f, -1.0f,
       1.0f,  1.0f, -1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
      -1.0f,  1.0f,  1.0f,
      -1.0f,  1.0f, -1.0f,

      -1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f,  1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f,  1.0f,
       1.0f, -1.0f,  1.0f
    };

    auto envmap_mesh = driver.make_mesh_repr();
    auto envmap_data_buf =
      envmap_mesh->allocate_buffer(env_cube_data.size() * sizeof(float),
                                   Usage_Hint::Draw, Upload_Hint::Static);
    envmap_mesh->format_buffer(envmap_data_buf, 0, 3, Buffer_Format::Float, 0, 0);
    envmap_mesh->enable_vertex_attrib(0);
    envmap_mesh->set_primitive_type(Primitive_Type::Triangle);
    envmap_mesh->buffer_data(envmap_data_buf, 0,
                             sizeof(float) * env_cube_data.size(),
                             &env_cube_data[0]);

    auto envmap_shader = driver.make_shader_repr();
    envmap_shader->load_vertex_part("shader/envmap/vs.glsl");
    envmap_shader->load_fragment_part("shader/envmap/fs.glsl");

    auto envmap_view_loc = envmap_shader->get_location("view");
    auto envmap_proj_loc = envmap_shader->get_location("proj");
    auto envmap_cube_loc = envmap_shader->get_location("envmap");
    envmap_shader->set_integer(envmap_cube_loc, 0);

    // Find the depth of the four corner points.
    // Unproject each point with the inverse proj * view matrix.

    auto projectile_mesh = gfx::load_mesh(driver, {"obj/projectile.obj", false});
    auto boat_mesh = gfx::load_mesh(driver, {"obj/boat.obj", false});

    auto boat_motion = collis::Motion{};
    boat_motion.mass = 64; // kilograms
    boat_motion.angular.radius = 0.5f; // Radius of about half a meter
    // Start the boat off in the middle of our water
    boat_motion.displacement.displacement = glm::vec3(0.0f, 0.5f, 0.0f);
    glm::mat4 boat_model{1.0f};

    // Make an fps camera.
    gfx::Camera cam;
    cam.projection_mode = gfx::Camera_Type::Perspective;
    cam.perspective =
      gfx::Perspective_Cam_Params{glm::radians(90.0f),
                                  driver.window_extents().x /
                                    (float) driver.window_extents().y,
                                  .001f, 1000.0f};
    cam.definition = gfx::Camera_Definition::Look_At;
    cam.look_at.up = glm::vec3(0.0f, 1.0f, 0.0f);

    // The eye will be rotated around the boat.
    glm::quat eye_dir;
    cam.look_at.eye = boat_motion.displacement.displacement - glm::vec3(0.0f, 5.0f, -6.0f);
    cam.look_at.look = boat_motion.displacement.displacement;

    // Make a grid and upload it
    auto water_grid = water::gen_grid(200);
    water::Plane water_base{{0.0f, 1.0f, 0.0f}, 0.0f};

    auto grid_mesh = driver.make_mesh_repr();
    auto grid_uv_buf =
      grid_mesh->allocate_buffer(sizeof(float) * 2 * water_grid.size(),
                                 redc::Usage_Hint::Draw,
                                 redc::Upload_Hint::Static);
    grid_mesh->format_buffer(grid_uv_buf, 0, 2, Buffer_Format::Float, 0, 0);
    grid_mesh->enable_vertex_attrib(0);
    grid_mesh->buffer_data(grid_uv_buf, 0, sizeof(float) * 2 * water_grid.size(),
                           &water_grid[0]);
    grid_mesh->set_primitive_type(Primitive_Type::Triangle);

    // Initialize the shader
    auto water_shader = driver.make_shader_repr();
    water_shader->load_vertex_part("shader/water/vs.glsl");
    water_shader->load_fragment_part("shader/water/fs.glsl");

    auto plane_loc = water_shader->get_location("plane");
    water_shader->set_vec4(plane_loc, plane_as_vec4(water_base));

    auto time_loc = water_shader->get_location("time");
    water_shader->set_float(time_loc, 0.0f);

    water_shader->set_integer(water_shader->get_location("octaves_in"), 5);
    water_shader->set_float(water_shader->get_location("amplitude_in"), 0.2f);
    water_shader->set_float(water_shader->get_location("frequency_in"), 0.5f);
    water_shader->set_float(water_shader->get_location("persistence_in"), 0.5f);
    water_shader->set_float(water_shader->get_location("lacunarity_in"), 0.6f);

    float max_displacement = 0.1f * std::pow(2.0f, 8);
    //auto displ_loc = water_shader->get_location("disp");
    //water_shader->set_float(displ_loc, max_displacement);

    auto projector_loc = water_shader->get_location("projector");
    water_shader->set_view_name("view");
    water_shader->set_projection_name("proj");

    water_shader->set_matrix(projector_loc, glm::mat4(1.0f));
    water_shader->set_view(glm::mat4(1.0f));
    water_shader->set_projection(glm::mat4(1.0f));

    auto cam_pos_loc = water_shader->get_location("camera_pos");
    auto light_dir_loc = water_shader->get_location("light_dir");

    auto water_envmap_loc = water_shader->get_location("envmap");
    water_shader->set_integer(water_envmap_loc, 0);

    driver.bind_texture(*envtex, 0);

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

    // This only needs to be done once since the boat shader doesn't use any
    // textures.
    //driver.bind_texture(*heightmap_tex, 0);
    //driver.bind_texture(*normalmap_tex, 1);
    //driver.bind_texture(*diffusemap_tex, 2);

    std::vector<collis::Motion> projectiles;

    gfx::Camera water_cam = cam;

    float prev_time = glfwGetTime();
    while(!glfwWindowShouldClose(window))
    {
      ++fps;

      // Clear the forces on the boat
      collis::reset_force(boat_motion);

      // Handle events (likely will add a force or two.
      glfwPollEvents();

      // Use last frame's model matrix to calculate the boat's direction.
      glm::vec3 boat_dir =
        glm::vec3(boat_render.model * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));

      if(glfw_user_data.forward_pressed == true)
      {
        collis::apply_force(boat_motion, boat_dir * -90.0f);
      }
      if(glfw_user_data.back_pressed == true)
      {
        collis::apply_force(boat_motion, boat_dir * 90.0f);
      }
      if(glfw_user_data.left_pressed == true)
      {
        boat_motion.angular.net_torque +=
          glm::cross(glm::vec3(0.0f, 0.0f, 1.0f),
                     glm::vec3(18.0f * 3.14f, 0.0f, 0.0f));
      }
      if(glfw_user_data.right_pressed == true)
      {
        boat_motion.angular.net_torque +=
          glm::cross(glm::vec3(0.0f, 0.0f, 1.0f),
                     glm::vec3(-18.0f * 3.14f, 0.0f, 0.0f));
      }
      if(glfw_user_data.update_cam == true)
      {
        water_cam = cam;
      }

      // Take the velocity of the boat and use it to calculate a drag force.
      // Just for shits n' giggles lets simply set a maximum velocity like this
      auto p = 971.8f; // Density of water at 80 deg C I think. kg / m^3
      auto v = glm::length(boat_motion.displacement.velocity); // m/s
      auto C_d = 0.04f; // I think a boat should be pretty low. No unit?
      auto A = 0.12f; // This less of a random guess
      glm::vec3 f_d = (.5f * p * v * v * C_d * A) *
        glm::normalize(-boat_motion.displacement.velocity);

      // Apply the drag force.
      if(!std::isnan(glm::length(f_d)))
      {
        collis::apply_force(boat_motion, f_d);
      }

      // Add rotational drag
      v = glm::length(boat_motion.angular.velocity);
      // We are moving sideways so there should be more area.
      A = 0.36f;
      f_d = (.5f * p * v * v * .7f * A) *
        glm::normalize(-boat_motion.angular.velocity);
      if(!std::isnan(glm::length(f_d)))
      {
        boat_motion.angular.net_torque += f_d;
      }

      // Do any projectiles
      if(glfw_user_data.should_spawn_projectile)
      {
        glfw_user_data.should_spawn_projectile = false;

        projectiles.emplace_back();
        projectiles.back().mass = 270;
        // We should instead retrieve this from the model.
        projectiles.back().angular.radius = 0.14f;

        projectiles.back().displacement = boat_motion.displacement;
        projectiles.back().angular = boat_motion.angular;

        collis::reset_force(projectiles.back());

        // Give the cannonball an initial velocity.
        // We should determine this using energy calculations later.
        projectiles.back().displacement.velocity =
          glm::rotate(glm::vec3(6.0f, 24.5f, 0.0f),
                      glm::length(boat_motion.angular.displacement),
                      glm::normalize(boat_motion.angular.displacement)) +
          boat_motion.displacement.velocity;

        // Apply the force of gravity
        collis::apply_force(projectiles.back(), projectiles.back().mass *
                            glm::vec3(0.0f, -9.81, 0.0f));

        // Apply the opposite momentum to the boat
      }

      // Solve the motion
      auto new_time = glfwGetTime();

      // For the boat
      solve_motion(new_time - prev_time, boat_motion);

      // For the projectiles
      for(auto& proj : projectiles)
      {
        solve_motion(new_time - prev_time, proj);
      }
      prev_time = new_time;

      // First mark the distance the camera was from the boat.
      auto cam_dist = glm::length(cam.look_at.eye - cam.look_at.look);
      // Calculate the camera position and look direction based on the location
      // of the boat
      cam.look_at.look = boat_motion.displacement.displacement;

      double x, y;
      glfwGetCursorPos(window, &x, &y);

      float dif_x = (x - prev_x) / 250.0f;
      float dif_y = (y - prev_y) / 250.0f;

      eye_dir = glm::normalize(eye_dir);


      auto cross_eye =
        glm::cross(glm::normalize(cam.look_at.look - cam.look_at.eye),
                   cam.look_at.up);
      eye_dir = eye_dir * glm::rotate(glm::quat(), dif_x, glm::inverse(eye_dir) * cam.look_at.up);
      eye_dir = eye_dir * glm::rotate(glm::quat(), dif_y, glm::inverse(eye_dir) * cross_eye);
      prev_x = x, prev_y = y;

      // Rotate the forward vector by the eye direction and ad it's location.
      cam.look_at.eye =
        glm::vec3(glm::mat4_cast(eye_dir) *
                  glm::vec4(0.0f, 0.0f, cam_dist, 1.0f)) + cam.look_at.look;

      // Calculate a model
      boat_render.model = glm::mat4(1.0f);
      boat_render.model = glm::translate(boat_render.model,
        boat_motion.displacement.displacement);
      if(!std::isnan(glm::length(boat_motion.angular.displacement)) &&
         glm::length(boat_motion.angular.displacement) != 0.0f)
      {
        boat_render.model = glm::rotate(boat_render.model,
            glm::length(boat_motion.angular.displacement),
            glm::normalize(boat_motion.angular.displacement));
      }

      use_camera(driver, cam);

      // Clear the screen
      driver.clear();

      // Render the environment map
      driver.use_shader(*envmap_shader);

      glm::mat4 env_camera_mat = camera_view_matrix(cam);
      // Zero out the translation
      env_camera_mat[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
      envmap_shader->set_matrix(envmap_view_loc, env_camera_mat);
      envmap_shader->set_matrix(envmap_proj_loc, camera_proj_matrix(cam));

      driver.write_depth(false);
      envmap_mesh->draw_arrays(0, env_cube_data.size() / 3);
      driver.write_depth(true);

      driver.use_shader(*shader);

      // Render the hull
      shader->set_model(boat_render.model);
      gfx::render_chunk(boat_render.hull);

      shader->set_model(glm::translate(boat_render.model, boat_render.attachments.sail));
      gfx::render_chunk(boat_render.sail);
      shader->set_model(glm::translate(boat_render.model, boat_render.attachments.rudder));
      gfx::render_chunk(boat_render.rudder);
      shader->set_model(glm::translate(boat_render.model, boat_render.attachments.gun));
      gfx::render_chunk(boat_render.gun);

      for(auto const& proj : projectiles)
      {
        glm::mat4 proj_model = glm::translate(glm::mat4(1.0f), proj.displacement.displacement);
        shader->set_model(proj_model);
        gfx::render_chunk(projectile_mesh.chunk);
      }

      // Render water

      driver.use_shader(*water_shader);
      use_camera(driver, cam);
      water_shader->set_vec3(cam_pos_loc, cam.look_at.eye);

      water_shader->set_vec3(light_dir_loc,
        glm::normalize(glm::vec3(5.0f, 5.0f, -6.0f)));

      auto intersections = water::find_visible(water_cam, water_base.dist, max_displacement);
      if(intersections.size())
      {
        auto projector = build_projector(water_cam, water_base, max_displacement);
        auto range = build_min_max_mat(intersections, projector, water_base);
        projector = projector * range;

        water_shader->set_matrix(projector_loc, projector);
        water_shader->set_float(time_loc, glfwGetTime());

        grid_mesh->draw_arrays(0, water_grid.size());
      }
      driver.use_shader(*shader);

      glfwSwapBuffers(window);

      if(int(glfwGetTime()) != time)
      {
        time = glfwGetTime();
        log_d("fps: %", fps);
        fps = 0;
      }
    }
  }

  flush_log_full();

  glfwTerminate();
  return 0;
}

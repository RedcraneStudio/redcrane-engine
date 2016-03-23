/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#include <iostream>
#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "common/log.h"
#include "use/mesh.h"
#include "gfx/gl/driver.h"
#include "gfx/camera.h"
#include "fps/camera_controller.h"
#include "use/texture.h"

#include "redcrane.hpp"
#include "minilua.h"

#include "sdl_helper.h"

namespace po = boost::program_options;

enum class Server_Mode
{
    Bad, Dedicated, Connect, Local
};

Server_Mode pick_server_mode(po::variables_map const& vm)
{
  bool dedicated = vm.count("dedicated-server");
  bool local = vm.count("local-server");
  bool connect = vm.count("connect");

  int total = dedicated + local + connect;
  if(total > 1)
  {
    return Server_Mode::Bad;
  }

  if(dedicated) return Server_Mode::Dedicated;
  if(connect) return Server_Mode::Connect;
    // By default, start a local server.
  else return Server_Mode::Local;
}

po::options_description command_options_desc() noexcept
{
  po::options_description general_opt("General");
  general_opt.add_options()
          ("help", "display help")
          ("out-log-level", po::value<unsigned int>()->default_value(2),
          "set minimum log level to stdout")
          ("log-file", po::value<std::string>(), "set log file")
          ("file-log-level", po::value<unsigned int>()->default_value(0),
          "set minimum log level to file")
          ("config-file", po::value<std::string>(), "config file to use")
  ;

  po::options_description server_opt("Networking");
  server_opt.add_options()
          ("name", po::value<std::string>(), "client name")
          ("port", po::value<uint16_t>()->default_value(28222),
           "set port number")
          ("max-peers", po::value<uint16_t>()->default_value(12),
           "set max number of connections")
          ("local-server", "start local server with a client gui")
          ("dedicated-server", "start a dedicated server without a client gui")
          ("advertise-server", "advertise the server to other clients")
          ("connect", po::value<std::string>(), "connect to a server")
  ;

  po::options_description config_opt("Config");
  config_opt.add_options()("render.fov", po::value<float>(),
                           "Field-of-view");

  config_opt.add_options()("render.model_quality", po::value<int>(),
                           "Baseline model quality");
  config_opt.add_options()("render.texture_quality", po::value<int>(),
                           "Baseline texture quality");
  config_opt.add_options()("render.water_quality", po::value<int>(),
                           "Baseline water quality");

  config_opt.add_options()("hud.scale", po::value<float>(), "Hud scale");

  config_opt.add_options()("game.cwd", po::value<std::string>(),
                           "Current working directory");

  config_opt.add_options()("game.entry_file", po::value<std::string>(),
                           "Lua main file");

  config_opt.add_options()("game.window_title", po::value<std::string>(),
                           "Window title");

  po::options_description desc("Allowed Options");

  desc.add(general_opt).add(server_opt).add(config_opt);

  return desc;
}

template <class T>
double time_since(T before) noexcept
{
  T now = std::chrono::high_resolution_clock::now();
  using sec_t = std::chrono::duration<double, std::chrono::seconds::period>;
  return sec_t(now - before).count();
}

int main(int argc, char* argv[])
{
  // At this point we assume we are in a directory where the config can be
  // found, otherwise it should be specified on the command line.

  using namespace redc;

  // Initialize logger
  Scoped_Log_Init log_init_raii_lock{};

  // Parse command line options
  po::variables_map vm;
  auto command_desc = command_options_desc();
  po::store(po::parse_command_line(argc, argv, command_desc), vm);

  if(vm.count("help"))
  {
    std::cerr << command_desc << std::endl;
    return EXIT_SUCCESS;
  }

  namespace fs = boost::filesystem;

  // If the user specified a config file on the command line use that one
  auto cfg_path = fs::path("cfg.ini");
  if(vm.count("config-file"))
  {
    cfg_path = fs::path(vm["config-file"].as<std::string>());
  }
  if(!fs::exists(cfg_path))
  {
    log_e("Config % doesn't exist", cfg_path.native());
    return EXIT_FAILURE;
  }

  // Otherwise we have a configuration file
  po::store(po::parse_config_file<char>(cfg_path.native().c_str(),
                                        command_desc), vm);

  // I don't think we need this but maybe.
  po::notify(vm);

  // Set log severities
  set_out_log_level((Log_Severity) vm["out-log-level"].as<unsigned int>());
  set_file_log_level((Log_Severity) vm["file-log-level"].as<unsigned int>());

  // Log file
  if(vm.count("log-file"))
  {
    set_log_file(vm["log-file"].as<std::string>());
  }

  // Everything we need is in vm now.

  // First change directory
  auto game_cwd =
          cfg_path.parent_path() / fs::path(vm["game.cwd"].as<std::string>());
  if(!exists(game_cwd))
  {
    log_e("Failed to enter game directory '%': it doesn't exist!",
          game_cwd.native());
    return EXIT_FAILURE;
  }
  current_path(game_cwd);

  // Initialize LuaJIT
  lua::Scoped_Lua_Init lua_init_raii_lock{};
  auto lua = lua_init_raii_lock.lua;

  // Everything alright?
  if(!lua)
  {
    log_e("Failed to initialize LuaJIT. This is generally caused by a memory"
                  "allocation error");
  }

  // Load a file
  auto entry_file = vm["game.entry_file"].as<std::string>();
  if(lua::handle_err(lua, luaL_loadfile(lua, entry_file.data())))
  {
    // Rip
    return EXIT_FAILURE;
  }

  SDL_Init_Lock sdl_init_raii_lock{vm["game.window_title"].as<std::string>(),
                                   {1000,1000}, false, false};
  auto sdl_window = sdl_init_raii_lock.window;

  SDL_SetRelativeMouseMode(SDL_TRUE);

  int x, y;
  SDL_GetWindowSize(sdl_window, &x, &y);

  gfx::gl::Driver driver{{x, y}};

#if 0

  auto eng = redc::Engine{driver, sdl_window};

  lua_pushlightuserdata(lua, &eng);

  if(lua::handle_err(lua, lua_pcall(lua, 1, 1, 0)))
  {
    return EXIT_FAILURE;
  }

  // The main script is all finished
  if(!lua_isnumber(lua, -1))
  {
    // We are either getting a string that is a valid integer, or something
    // completely ridiculous. This is not an valid issue to crash, but still is
    // incorrect.
    log_w("Main game Lua entry point must return an integer");
  }

  // Now get the return value as an integer and return that
  int ret = lua_tointeger(lua, -1);

#endif

  btDefaultCollisionConfiguration bt_config;
  btCollisionDispatcher bt_dispatcher{&bt_config};
  btDbvtBroadphase bt_broadphase;
  btSequentialImpulseConstraintSolver bt_solver;

  btDiscreteDynamicsWorld bt_world{&bt_dispatcher, &bt_broadphase, &bt_solver,
                                   &bt_config};

  bt_world.setGravity(btVector3(0.0f, -9.81f, 0.0f));

  btStaticPlaneShape bt_plane{btVector3(0.0f, 1.0f, 0.0f), 0.0f};
  btScalar plane_mass = 0.;
  btTransform plane_transform;
  plane_transform.setIdentity();
  btDefaultMotionState plane_state{plane_transform};

  btRigidBody::btRigidBodyConstructionInfo rb_info{plane_mass, &plane_state,
                                                   &bt_plane};

  btRigidBody bt_plane_body(rb_info);
  bt_world.addRigidBody(&bt_plane_body);

  btSphereShape sphere_shape(0.45f);
  btScalar sphere_mass = 5.;
  btTransform sphere_transform;
  sphere_transform.setIdentity();
  sphere_transform.setOrigin(btVector3(0.0f, 5.0f, -5.0f));
  btDefaultMotionState sphere_state{sphere_transform};

  btRigidBody::btRigidBodyConstructionInfo sphere_cb_info{sphere_mass,
                                                          &sphere_state,
                                                          &sphere_shape};
  btRigidBody bt_sphere_body(sphere_cb_info);
  bt_sphere_body.setRestitution(1.0f);
  bt_plane_body.setRestitution(.8f);
  bt_world.addRigidBody(&bt_sphere_body);

  // Build camera
  auto cam = gfx::make_fps_camera(driver);
  cam.perspective.aspect = 1.0;
  cam.perspective.far = 1000.0f;
  cam.perspective.near = 0.01f;
  cam.perspective.fov = glm::radians(68.0f);
  cam.fp.pos = glm::vec3(0.0f, 0.5f, 0.0f);

  auto cam_controller = fps::Camera_Controller{};
  cam_controller.camera(cam);
  cam_controller.set_pitch_limit(M_PI / 2, true);

  // Load objects
  auto plane = gfx::load_mesh(driver, {"../assets/obj/plane.obj", false}).chunk;
  auto sphere = gfx::load_mesh(driver, {"../assets/obj/sphere.obj", false}).chunk;

  // Load shader
  auto shader = driver.make_shader_repr();
  shader->load_vertex_part("../assets/shader/basic/vs.glsl");
  shader->load_fragment_part("../assets/shader/basic/fs.glsl");

  shader->set_model_name("model");
  shader->set_view_name("view");
  shader->set_projection_name("proj");
  shader->set_sampler_name("tex");
  shader->set_diffuse_name("dif");

  auto light_loc = shader->get_location("light_pos");
  shader->set_vec3(light_loc, glm::vec3(0.0f, 10.0f, 0.0f));

  shader->set_diffuse(colors::white);

  driver.use_shader(*shader);

  auto tex = gfx::load_texture(driver, "../assets/tex/sand.png");
  driver.bind_texture(*tex, 0);

  auto before = std::chrono::high_resolution_clock::now();

  driver.clear_color_value(colors::white);

  bool running = true;
  while(running)
  {
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
      switch(event.type)
      {
        case SDL_QUIT:
          running = false;
          break;
        case SDL_MOUSEMOTION:
          cam_controller.apply_delta_yaw(event.motion.xrel / 1000.0f);
          cam_controller.apply_delta_pitch(event.motion.yrel / 1000.0f);
          break;
      }
    }

    float dt = time_since(before);
    before = std::chrono::high_resolution_clock::now();

    bt_world.stepSimulation(dt, 10);

    driver.clear();
    gfx::use_camera(driver, cam);

    shader->set_sampler(0);

    auto model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(5.0f, 1.0f, 5.0f));
    shader->set_model(model);
    gfx::render_chunk(plane);

    model = glm::mat4(1.0f);

    btTransform sphere_transform;
    sphere_state.getWorldTransform(sphere_transform);
    model = glm::translate(model, glm::vec3(sphere_transform.getOrigin().x(),
                                            sphere_transform.getOrigin().y(),
                                            sphere_transform.getOrigin().z()));

    log_i("%", model[3][1]);

    shader->set_model(model);
    gfx::render_chunk(sphere);

    SDL_GL_SwapWindow(sdl_window);
  }

  return EXIT_SUCCESS;
}

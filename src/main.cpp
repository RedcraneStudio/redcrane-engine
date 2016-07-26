/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#include <iostream>
#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include "common/log.h"
#include "use/mesh.h"
#include "use/mesh_cache.h"
#include "gfx/gl/driver.h"
#include "gfx/camera.h"
#include "fps/camera_controller.h"
#include "use/texture.h"

#include "assets/load_dir.h"

#include "player.h"
#include "cwrap/redcrane.hpp"
#include "minilua.h"

#include "sdl_helper.h"

namespace po = boost::program_options;

enum class Server_Mode : int
{
    Bad = -1, Dedicated = 0, Local = 1, Connect = 2
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
          ("connect-timeout", po::value<float>()->default_value(10.0f),
           "set timeout time of connections")
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

  po::options_description desc("Allowed Options");

  desc.add(general_opt).add(server_opt).add(config_opt);

  return desc;
}
int start_connect(po::variables_map const& vm)
{
  using namespace redc;

  auto start_time = std::chrono::high_resolution_clock::now();

  // Get details
  auto port = vm["port"].as<uint16_t>();
  auto addr = vm["connect"].as<std::string>();

  // Initialize a client
  auto client = net::Client_Context{};

  // Set necessary fields
  enet_address_set_host(&client.server_addr, addr.c_str());
  client.server_addr.port = port;

  client.client_version = 0x01;

  // Now: Make this up, later: pick a team dynamically.
  client.player_info.name = "Luke";
  client.player_info.id = 0;
  client.player_info.team = 0;

  // Start connecting we need to do this first so ctx.host is initialized.
  step_client(client, nullptr);

  int ret_code = EXIT_SUCCESS;
  bool running_game = true;

  // For the actual sendrate
  double sendrate = 0.0;
  auto last_tick = std::chrono::high_resolution_clock::now();

  // TODO: I cri when I see this code.
  // This is the initial connection loop, it connects until the server is ready
  // for play, at which point we figure out the tickrate of the server and
  // calculate a sendrate for ourselves, that will be when the fun actually
  // begins.

  // While we still want to run but also the client isn't playing.
  // When we reach playing we need another loop
  while(running_game && client.state != net::Client_State::Playing)
  {
    // Time out if necessary
    if(client.state == net::Client_State::Connecting &&
       time_since(start_time) > vm["connect-timeout"].as<float>())
    {
      // We've waited some time, fuck it
      log_e("Failed to connect to %:%", addr, port);
      running_game = false;
      ret_code = EXIT_FAILURE;
      break;
    }

    ENetEvent event;
    if(enet_host_service(client.host.host, &event, 10))
    {
      // Step the client with the event
      auto res = step_client(client, &event);

      if(res.context_changed)
      {
        switch(client.state)
        {
          case net::Client_State::Playing:
            // Do playing things
            log_i("All done connecting, starting game.");

            // Find our sendrate.
            // Sample input every 1 / tickrate seconds.
            sendrate = 1.0f / client.server_info.rules.tickrate;
            break;
          case net::Client_State::Bad_Version:
            log_e("Server returned bad version");
            running_game = false;
            ret_code = EXIT_FAILURE;
            break;
          default:
            break;
        }
      }

      if(!res.event_handled)
      {
        if(event.type == ENET_EVENT_TYPE_DISCONNECT)
        {
          // Get the hell out!
          log_i("Disconnected from server");
          running_game = false;
          break;
        }
      }
    }
    else
    {
      net::step_client(client, nullptr);
    }
  }

  // At this point we may initialize the window

  // Persistent Input
  Input_Config input_config = get_default_input_config();
  Input input;
  while(running_game)
  {
    // Handle network events, make sure our (networked) environment is sane.
    // In other words the server hasn't disconnected from us, otherwise writing
    // data to it could cause a memory leak.
    ENetEvent enet_event;
    while(enet_host_service(client.host.host, &enet_event, 1))
    {
      // Read state?
      switch(enet_event.type)
      {
        case ENET_EVENT_TYPE_CONNECT:
          break;
        case ENET_EVENT_TYPE_RECEIVE:
          enet_packet_destroy(enet_event.packet);
          break;
        case ENET_EVENT_TYPE_DISCONNECT:
          log_i("Disconnected from server");

          // The fun is over
          running_game = false;

          // Just to be safe
          client.server_peer = nullptr;

          break;
        case ENET_EVENT_TYPE_NONE:
        default:
          break;
      }
    }

    // Handle SDL events
    SDL_Event sdl_event;
    while(SDL_PollEvent(&sdl_event))
    {
      if(!collect_input(input, sdl_event, input_config))
      {
        // Any other use for input?
      }
    }

    // Possibly send sample of the input?
    if(time_since(last_tick) > sendrate)
    {
      // Send
      if(client.server_peer)
      {
        net::send_data(input, client.server_peer, false);
      }
      else
      {
        log_d("Did not send input to server because the server disconnected, "
              "no problem");
      }
    }

    // Render

    // Sleep
    SDL_Delay(1);
  }

  return ret_code;
}
int start_dedicated(po::variables_map const& vm)
{
  using namespace redc;

  log_w("Dedicated mode not yet implemented");

  // Get details
  auto port = vm["port"].as<uint16_t>();
  auto max_peers = vm["max-peers"].as<uint16_t>();

  net::Server_Context server;
  server.port = port;
  server.max_peers = max_peers;
  net::init_server(server);

  bool running = true;
  while(running)
  {
    ENetEvent event;
    // For right now this is all we are doing so feel free to wait for a while
    while(enet_host_service(server.host.host, &event, 100))
    {
      net::step_server(server, event);
    }
  }

  return EXIT_SUCCESS;
}
int start_local(po::variables_map const& vm)
{
#if 0
  using namespace redc;

  // Load a file
  auto entry_file = vm["game.client_entry"].as<std::string>();
  if(lua::handle_err(lua, luaL_loadfile(lua, entry_file.data())))
  {
    // Rip
    return EXIT_FAILURE;
  }

  // Use the sandbox
  lua::set_sandbox_env(lua, 1);

  // Call the file
  if(lua::handle_err(lua, lua_pcall(lua, 0, 1, 0)))
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

  return ret;
#endif
#if 0
  btDefaultCollisionConfiguration bt_config;
  btCollisionDispatcher bt_dispatcher{&bt_config};
  btDbvtBroadphase bt_broadphase;
  btSequentialImpulseConstraintSolver bt_solver;

  btDiscreteDynamicsWorld bt_world{&bt_dispatcher, &bt_broadphase, &bt_solver,
                                   &bt_config};

  // 9.81 meters per second squared!
  bt_world.setGravity(btVector3(0.0f, -9.81f, 0.0f));

  // Add static plane
  btStaticPlaneShape floor_shape{btVector3(0.0f, 1.0f, 0.0f), 0.0f};

  // It should not be transformed.
  btTransform plane_transform;
  plane_transform.setIdentity();
  btDefaultMotionState floor_state{plane_transform};

  // Add the plane
  btRigidBody::btRigidBodyConstructionInfo floor_rbc_info{
          0.0f, &floor_state, &floor_shape
  };
  btRigidBody floor_body(floor_rbc_info);
  floor_body.setFriction(1.0f);
  bt_world.addRigidBody(&floor_body);

  Player_Controller player_controller;
  bt_world.addAction(&player_controller);

  btSphereShape ball_shape(0.75f);

  btTransform ball_init_transform;
  ball_init_transform.setIdentity();
  ball_init_transform.setOrigin(btVector3(0.0f, 1.0f, -5.0f));

  btDefaultMotionState ball_state{ball_init_transform};

  btRigidBody::btRigidBodyConstructionInfo ball_rbc_info{
          5.f, &ball_state, &ball_shape
  };
  btRigidBody ball_body(ball_rbc_info);
  bt_world.addRigidBody(&ball_body);

  // Build camera
  auto cam = gfx::make_fps_camera(driver);
  cam.perspective.aspect = 1.0;
  cam.perspective.far = 1000.0f;
  cam.perspective.near = 0.01f;
  cam.perspective.fov = glm::radians(68.0f);
  cam.fp.pos = glm::vec3(5.0f, 1.0f, 3.0f);
  //cam.fp.pitch = (float) M_PI / 2;

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

  auto begin = std::chrono::high_resolution_clock::now();
  auto before = std::chrono::high_resolution_clock::now();

  driver.clear_color_value(colors::white);

  Input input;
  auto input_cfg = get_default_input_config();

  // Yuck. TODO: Make this awesome with maybe_owned or something.
  player_controller.set_input_ref(&input);

  int last_sec = 0;
  int frames = 0;

  bool running = true;
  while(running)
  {
    int second_now = (int) time_since(begin);
    if(second_now != last_sec)
    {
      log_i("fps %", frames);
      last_sec = second_now;
      frames = 0;
    }
    else
    {
      ++frames;
    }


    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
      // If we already used the event, bail.
      if(collect_input(input, event, input_cfg)) continue;

      // Otherwise
      switch(event.type)
      {
        case SDL_QUIT:
          running = false;
          break;
        case SDL_MOUSEMOTION:
          cam_controller.apply_delta_yaw(event.motion.xrel / 1000.0f);
          cam_controller.apply_delta_pitch(event.motion.yrel / 1000.0f);
          break;
        case SDL_KEYDOWN:
          if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) running = false;
          break;
        default:
          break;
      }
    }

    float dt = time_since(before);
    before = std::chrono::high_resolution_clock::now();

    if(time_since(begin) > 2.0) bt_world.stepSimulation(dt, 10);

    driver.clear();

    // Figure out where we are at and put the camera there
    btTransform personTransform;
    player_controller.getWorldTransform(personTransform);
    cam.fp.pos.x = personTransform.getOrigin().getX();
    cam.fp.pos.y = personTransform.getOrigin().getY();
    cam.fp.pos.z = personTransform.getOrigin().getZ();

    // We actually don't care about this value, unfortunately we can't *not*
    // get it
    //float roll = 0.0;
    //personTransform.getBasis().getEulerYPR(cam.fp.yaw, cam.fp.pitch, roll);

    gfx::use_camera(driver, cam);

    shader->set_sampler(0);

    auto model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(5.0f, 1.0f, 5.0f));
    shader->set_model(model);
    gfx::render_chunk(plane);

    model = glm::mat4(1.0f);

    btTransform sphere_transform;
    ball_state.getWorldTransform(sphere_transform);
    sphere_transform.getOpenGLMatrix(glm::value_ptr(model));

    //log_i("%", sphere_transform.getOrigin().getZ());

    shader->set_model(model);
    gfx::render_chunk(sphere);

    SDL_GL_SwapWindow(sdl_window);
  }

  return EXIT_SUCCESS;
#endif
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
  // I don't think we need this but maybe.
  po::notify(vm);

  if(vm.count("help"))
  {
    std::cerr << command_desc << std::endl;
    return EXIT_SUCCESS;
  }

  // Log file
  if(vm.count("log-file"))
  {
    set_log_file(vm["log-file"].as<std::string>());
  }

  // Set log severities
  set_out_log_level((Log_Severity) vm["out-log-level"].as<unsigned int>());
  set_file_log_level((Log_Severity) vm["file-log-level"].as<unsigned int>());

  // Initialize LuaJIT
  lua::Scoped_Lua_Init lua_init_raii_lock{};
  auto lua = lua_init_raii_lock.lua;

  // Everything alright?
  if(!lua)
  {
    log_e("Failed to initialize LuaJIT. This is generally caused by a memory"
                  "allocation error");
  }

  // First load c functions into ffi.C
  if(lua::handle_err(lua, lua::load_c_functions(lua)))
  {
    return EXIT_FAILURE;
  }

  namespace fs = boost::filesystem;

  // Check for config file
  if(!vm.count("config-file"))
  {
    log_e("No config file specified");
    return EXIT_FAILURE;
  }

  // Check to make sure it exists
  auto cfg_path = fs::path(vm["config-file"].as<std::string>());
  if(!fs::exists(cfg_path))
  {
    log_e("Config % doesn't exist", cfg_path.native());
    return EXIT_FAILURE;
  }

  // Make the configuration path absolute so we can be sure that it will still
  // point to a valid filename after switching directories.
  cfg_path = absolute(cfg_path);

  // Switch to its directory
  current_path(cfg_path.parent_path());

  // If there is no error, we should have the engine on the top of the lua
  // stack now.
  // Now we have a configuration file, preload our API and config file this
  // does both because the whole operation is pretty coupled.
  if(lua::handle_err(lua, lua::load_engine_lua(lua, cfg_path)))
  {
    return EXIT_FAILURE;
  }

  // Pick the server mode
  auto server_mode = pick_server_mode(vm);

  int ret_code = EXIT_SUCCESS;

  switch(server_mode)
  {
    case Server_Mode::Dedicated:
      ret_code = lua::run_engine(lua, "dedicated");
      break;
    case Server_Mode::Connect:
      ret_code = lua::run_engine(lua, "connect");
      break;
    case Server_Mode::Local:
      ret_code = lua::run_engine(lua, "local");
      break;
    default:
    case Server_Mode::Bad:
      log_e("Must pick either dedicated, local or connect server mode");
      ret_code = EXIT_FAILURE;
      break;
  }

  return ret_code;
}

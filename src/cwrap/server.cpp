/*
 * Copyright (c) 2016 Luke San Antonio
 * All rights reserved.
 *
 * This file provides the implementation for the engine's C interface,
 * specifically tailored for LuaJIT's FFI facilities.
 *
 * General engine stuff
 */

#include "redcrane.hpp"

using namespace redc;

namespace redc
{
  Server::Server()
  {
    bt_config = std::make_unique<btDefaultCollisionConfiguration>();
    bt_dispatcher = std::make_unique<btCollisionDispatcher>(bt_config.get());
    bt_broadphase = std::make_unique<btDbvtBroadphase>();
    bt_solver = std::make_unique<btSequentialImpulseConstraintSolver>();

    bt_world = std::make_unique<btDiscreteDynamicsWorld>(bt_dispatcher.get(),
                                                         bt_broadphase.get(),
                                                         bt_solver.get(),
                                                         bt_config.get());

    bt_world->setGravity(btVector3(0.0f, -9.81f, 0.0f));
  }
}

obj_id redc_server_make_player(void *eng)
{
  auto rce = (Engine*) eng;
  REDC_ASSERT_HAS_SERVER(rce);

  // Add player to list of players, return obj id
  auto id = rce->server->index_gen.get();

  // Add local player controller action interface to player controller.
  rce->server->bt_world->addAction(&rce->server->players[id-1].controller);

  if(!rce->server->active_player)
  {
    rce->server->active_player = id;
  }

  return id;
}

void redc_server_step(void *eng)
{
  auto rce = (Engine*) eng;
  REDC_ASSERT_HAS_SERVER(rce);

  Input input;
  rce->server->players[rce->server->active_player-1].controller.set_input_ref(&input);

  rce->server->bt_world->stepSimulation(time_since(rce->start_time), 10);
}

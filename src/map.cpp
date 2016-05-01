/*
 * Copyright (c) 2016 Luke San Antonio
 * All rights reserved.
 */
#include "map.h"
#include "cwrap/redcrane.hpp"
namespace redc
{
  Physics_Component::~Physics_Component()
  {
    server_->bt_world->removeRigidBody(this->body.get());
  }
}

/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <LinearMath/btMotionState.h>
#include <BulletDynamics/btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include "SDL.h"
#include "input/input.h"
namespace redc
{
  // Convert the movement keys from Input into internal position for Bullet and
  // for the netcode.
  struct Player_Movement : public btMotionState
  {
    // Get updated by the dynamics world
    void setWorldTransform(btTransform const& world) override;
    void getWorldTransform(btTransform& trans) const override;
  };

  struct Player_Controller : public btActionInterface
  {
    Player_Controller();

    void updateAction(btCollisionWorld* world, btScalar dt) override;
    void debugDraw(btIDebugDraw*) {}

    inline void set_input_ref(Input* input) { input_ref_ = input; }
    Input* get_input_ref() { return input_ref_; }

    inline btTransform getWorldTransform(btTransform& t) const
    { t = ghost_.getWorldTransform(); }
  private:
    bool inited_;
    btPairCachingGhostObject ghost_;
    btCapsuleShape shape_;

    Input* input_ref_;
  };
}
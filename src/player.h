/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <glm/glm.hpp>
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

  enum class Player_State
  {
    Grounded, Jumping, Flying
  };

  struct Player_Controller : public btActionInterface
  {
    Player_Controller();

    inline void reset() { inited_ = false; }

    void updateAction(btCollisionWorld* world, btScalar dt) override;
    void debugDraw(btIDebugDraw*) override {}

    inline void set_input_ref(Input* input) { input_ref_ = input; }
    Input* get_input_ref() { return input_ref_; }

    inline btTransform getWorldTransform(btTransform& t) const
    { t = ghost_.getWorldTransform(); return t; }

    void apply_delta_yaw(double dv);
    void apply_delta_pitch(double dv);

    glm::vec3 get_cam_pos() const;

    Player_State state = Player_State::Grounded;
  private:
    bool inited_;
    btPairCachingGhostObject ghost_;
    btCapsuleShape shape_;
    btGhostPairCallback ghost_cb_;

    Input* input_ref_;

    btVector3 jump_velocity_;

    btVector3 last_normal_;

    btQuaternion pitch_;
    btVector3 gun_target_;
  };

  struct Player
  {
    Player_Controller controller;

    // Player state (FSM)
  };
}

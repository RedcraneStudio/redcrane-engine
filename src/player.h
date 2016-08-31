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
#include "common/reactor.h"
#include "common/timer.hpp"
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
    Grounded, Jumping
  };

  // The player is made up of shoes, half a sphere, a cylinder and another
  // half-sphere. The sum of capsule_height, 2 * radius and shoe_size should be
  // equal to total_height.
  //
  // The fact that some of these properties may change and may not change is
  // probably important, I mean a change in mass won't be honored, so what's the
  // point in putting it here? Should we separate const properties from
  // non-const properties?
  struct Player_Properties
  {
    float total_height;
    float capsule_height;

    float radius;
    float shoe_size;

    float mass;
    float speed;

    // Amount of time/s between steps
    float step_rate;

    bool is_crouched;
  };

  struct Player_Event
  {
    enum
    {
      Footstep,
      Jump,
      Crouch
    } type;
  };

  struct Server;
  struct Player_Controller : public btActionInterface
  {
    Player_Controller();

    Server* server;

    inline void reset() { inited_ = false; }

    void updateAction(btCollisionWorld* world, btScalar dt) override;
    void debugDraw(btIDebugDraw*) override {}

    inline void set_input_ref(Input* input) { input_ref_ = input; }
    Input* get_input_ref() { return input_ref_; }

    inline btTransform getWorldTransform(btTransform& t) const
    { t = ghost_.getWorldTransform(); return t; }

    void apply_delta_yaw(double dv);
    void apply_delta_pitch(double dv);

    Player_Properties const& get_player_properties() const
    { return player_props_; }

    glm::vec3 get_player_pos() const;
    glm::vec3 get_cam_pos() const;

    inline bool poll_event(Player_Event& event)
    { return events_.poll_event(event); }

    Player_State state = Player_State::Grounded;
  private:
    bool inited_;
    btPairCachingGhostObject ghost_;
    btGhostPairCallback ghost_cb_;

    btCapsuleShape shape_;
    btCapsuleShape crouch_shape_;

    void set_normal_props();
    void set_crouch_props();
    Player_Properties player_props_;

    Input* input_ref_;

    btVector3 jump_velocity_;

    btVector3 last_normal_;

    btQuaternion pitch_;

    float target_height_;

    Timer<> walk_timer_;

    Queue_Event_Source<Player_Event> events_;
  };

  struct Player
  {
    Player_Controller controller;

    // Player state (FSM)
  };
}

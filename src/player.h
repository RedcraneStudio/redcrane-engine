/*
 * Copyright (C) 2016 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <LinearMath/btMotionState.h>
#include "SDL.h"
namespace redc
{
  // Abstraction of an input button, may be a mouse button or key.
  struct Button
  {
    enum {Keyboard, Mouse} type;

    union
    {
      SDL_Scancode key;
      uint8_t mouse;
    };
  };

  struct Input_Config
  {
    Button forward;
    Button backward;
    Button strafe_left;
    Button strafe_right;

    Button primary_attack;
    Button secondary_attack;
    Button tertiary_attack;
  };

  Input_Config get_default_input_config();

  // Sampled input (no mouse motion
  struct Input
  {
    bool forward;
    bool backward;

    bool strafe_left;
    bool strafe_right;

    bool primary_attack;
    bool secondary_attack;
    bool tertiary_attack;
  };

  // Returns true if the input was "collected" or used.
  bool collect_input(Input& input, SDL_Event const& event,
                     Input_Config const& cfg);

  // Convert the movement keys from Input into internal position for Bullet and
  // for the netcode.
  struct Player_Movement : public btMotionState
  {
    // Get updated by the dynamics world
    void setWorldTransform(btTransform const& world) override;
    void getWorldTransform(btTransform& trans) const override;
  };
}

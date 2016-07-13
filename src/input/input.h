/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <msgpack.hpp>
#include "SDL.h"
namespace redc
{
  // Sampled input (no mouse motion)
  struct Input
  {
    bool forward;
    bool backward;

    bool strafe_left;
    bool strafe_right;

    bool jump;
    bool crouch;

    bool primary_attack;
    bool secondary_attack;
    bool tertiary_attack;

    MSGPACK_DEFINE(forward, backward, strafe_left, strafe_right, jump, crouch,
                   primary_attack, secondary_attack, tertiary_attack);
  };

  // Abstraction of an input button, may be a mouse button or key.
  struct Button
  {
    enum
    {
      Keyboard, Mouse
    } type;

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

    Button jump;
    Button crouch;

    Button primary_attack;
    Button secondary_attack;
    Button tertiary_attack;
  };

  Input_Config get_default_input_config();

  // Returns true if the input was "collected" or used.
  bool collect_input(Input &input, SDL_Event const &event,
                     Input_Config const &cfg);

}

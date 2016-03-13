/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "input.h"
namespace redc
{
  // Optimizer pls halp
  inline bool check_button(SDL_Event const& event, Button button, bool& value)
  {
    // Check keyboard
    if(button.type == Button::Keyboard && event.type == SDL_KEYDOWN &&
       event.key.keysym.scancode == button.key)
    {
      value = true;
      return true;
    }
    if(button.type == Button::Keyboard && event.type == SDL_KEYUP &&
       event.key.keysym.scancode == button.key)
    {
      value = false;
      return true;
    }

    if(button.type == Button::Mouse && event.type == SDL_MOUSEBUTTONDOWN &&
       event.button.button == button.mouse)
    {
      value = true;
      return true;
    }
    else if(button.type == Button::Mouse && event.type == SDL_MOUSEBUTTONUP &&
            event.button.button == button.mouse)
    {
      value = false;
      return true;
    }

    // Not a valid key.
    return false;
  }

  bool collect_input(Input& input, SDL_Event const& event,
                     Input_Config const& cfg)
  {
    bool ret = false;

    // Check each button and figure out if at least one of them was handled.
    ret |= check_button(event, cfg.forward,          input.forward);
    ret |= check_button(event, cfg.backward,         input.backward);
    ret |= check_button(event, cfg.strafe_left,      input.strafe_left);
    ret |= check_button(event, cfg.strafe_right,     input.strafe_right);

    ret |= check_button(event, cfg.primary_attack,   input.primary_attack);
    ret |= check_button(event, cfg.secondary_attack, input.secondary_attack);
    ret |= check_button(event, cfg.tertiary_attack,  input.tertiary_attack);

    return ret;
  }

  Input_Config get_default_input_config()
  {
    Input_Config cfg;

#define SET_KEY(prop, scan) \
    cfg.prop.type = Button::Keyboard; \
    cfg.prop.key = scan
#define SET_MBUTTON(prop, btn) \
    cfg.prop.type = Button::Mouse; \
    cfg.prop.mouse = btn;

    // Standard WASD
    SET_KEY(forward, SDL_SCANCODE_W);
    SET_KEY(backward, SDL_SCANCODE_S);
    SET_KEY(strafe_left, SDL_SCANCODE_A);
    SET_KEY(strafe_right, SDL_SCANCODE_D);

    // Three-button mouse
    SET_MBUTTON(primary_attack, SDL_BUTTON_LEFT);
    SET_MBUTTON(secondary_attack, SDL_BUTTON_RIGHT);
    SET_MBUTTON(tertiary_attack, SDL_BUTTON_MIDDLE);

#undef SET_KEY
#undef SET_MBUTTON

    return cfg;
  }
}

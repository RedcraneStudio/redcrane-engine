/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "player_state.h"
#include "../map/map.h"
namespace game { namespace strat
{
  Player_State::Player_State(Game_State& g_state) noexcept
    : impl_(std::make_shared<Nothing_State>(*this, g_state)),
      g_state_(&g_state) {}

  void Nothing_State::step_mouse(Mouse_State const& ns) noexcept
  {
    if(ns.buttons & ui::Mouse_Button_Left)
    {
      gfx::apply_pan(g_state_->cam, ns.position, op_, *g_state_->driver);
    }
    op_ = ns.position;
  }

  void Building_State::step_mouse(Mouse_State const& ms) noexcept
  {
    g_state_->map.pending_structure = Structure_Instance{*st_, ms.position};
    // TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
    // TODO Put this check in a function along with other buttons so we TODO
    // TODO can't screw it up. Put this someplace accessible like       TODO
    // TODO mouse_logic file itself.                                    TODO
    // TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
    if(ms.buttons & ui::Mouse_Button_Left)
    {
      if(try_structure_place(g_state_->map, *st_, ms.position))
      {
        g_state_->map.pending_structure = boost::none;
        g_state_->map.structures.push_back({*st_, ms.position});

        p_state_->switch_state<Nothing_State>(ms.position);
      }
    }
  }
} }

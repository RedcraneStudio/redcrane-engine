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
    auto mouse_world = unproject(*g_state_, ms.position);
    auto map_pos = Vec<float>{mouse_world.x, mouse_world.z};

    g_state_->map.pending_structure = Structure_Instance{*st_, map_pos};
    // TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
    // TODO Put this check in a function along with other buttons so we TODO
    // TODO can't screw it up. Put this someplace accessible like       TODO
    // TODO mouse_logic file itself.                                    TODO
    // TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
    if(ms.buttons & ui::Mouse_Button_Left)
    {
      // Convert mouse to map coordinates
      if(try_structure_place(g_state_->map, *st_, map_pos))
      {
        g_state_->map.pending_structure = boost::none;
        g_state_->map.structures.push_back({*st_, map_pos});

        // Tell the nothing state where the mouse *was* so we don't get weird
        // jumps and zooms right after the switch.
        p_state_->switch_state<Nothing_State>(ms.position);
      }
    }
  }
} }

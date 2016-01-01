/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "player_state.h"
#include "map.h"
#include "../common/log.h"
namespace redc { namespace strat
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

    cam_rot_.step_mouse(ns);
    cam_orient_.step_mouse(ns);
    if(cam_orient_.done()) cam_orient_.reset();
  }

  void Building_State::step_mouse(Mouse_State const& ms) noexcept
  {
    auto mouse_world = unproject(*g_state_, ms.position);
    auto map_pos = Vec<float>{mouse_world.x, mouse_world.z};

    g_state_->map.pending_structure = Structure_Instance{*st_, map_pos};
    if(ms.buttons & ui::Mouse_Button_Left)
    {
      // Convert mouse to map coordinates
      if(try_structure_place(g_state_->map, *st_, map_pos))
      {
        g_state_->map.pending_structure = boost::none;

        // We don't need to insert the structure here because the try structure
        // place function already does that.
        // g_state_->map.structures.push_back({*st_, map_pos});

        // Tell the nothing state where the mouse *was* so we don't get weird
        // jumps and zooms right after the switch.
        p_state_->switch_state<Nothing_State>(ms.position);
      }
    }
  }

  void Wall_Building_State::step_mouse(Mouse_State const& state) noexcept
  {
    // TODO: Possibly refactor this code in a helper class or something.
    if(click_index_ == 0 && state.buttons & ui::Mouse_Button_Left)
    {
      ++click_index_;
    }
    if(click_index_ == 1 && !(state.buttons & ui::Mouse_Button_Left))
    {
      // New pending wall
      g_state_->map.pending_wall = Pending_Wall{};
      g_state_->map.pending_wall->type = wall_type_;

      // TODO: Do the world -> map transformation somewhere else.
      auto map_pos = unproject(*g_state_, state.position);
      g_state_->map.pending_wall->pos = {map_pos.x, map_pos.z};

      ++click_index_;
    }
    else if(click_index_ == 2 && state.buttons & ui::Mouse_Button_Left)
    {
      ++click_index_;
    }
    else if(click_index_ == 3 && !(state.buttons & ui::Mouse_Button_Left))
    {
      // World -> Map TODO: See above
      auto mouse_world = unproject(*g_state_, state.position);
      auto map_pos = Vec<float>{mouse_world.x, mouse_world.z};

      // Project the end map position onto the nearest axis.
      auto endpt = pending_wall_end_pt(g_state_->map.pending_wall.value(),
                                       map_pos);

      // Add a new wall and add each point from the pending wall.
      g_state_->map.walls.emplace_back();
      auto& wall = g_state_->map.walls.back();
      end_wall(g_state_->map.pending_wall.value(), wall, endpt);

      // No more pending wall.
      g_state_->map.pending_wall = boost::none;

      // We can place more walls.
      click_index_ = 0;

      // But actually let's just leave the game state
      p_state_->switch_state<Nothing_State>(state.position);
    }
  }
} }

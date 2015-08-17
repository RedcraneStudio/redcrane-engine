/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../gfx/camera.h"
#include "../gfx/support/unproject.h"
#include "../ui/mouse_logic.h"

#include "wall.h"
#include "map.h"
#include "structure.h"
namespace game { namespace strat
{
  // Deals with everything fairly global pertaining to actual gameplay.
  struct Game_State
  {
    gfx::IDriver* driver;
    gfx::Camera cam;
    Map map;
  };

  inline glm::vec3 unproject(Game_State const& g_state, Vec<int> pt) noexcept
  {
    return
      gfx::unproject_screen(*g_state.driver, g_state.cam, glm::mat4(1.0f), pt);
  }

  using ui::Mouse_State;

  struct Player_State;

  struct Player_State_Impl
  {
    Player_State_Impl(Player_State& p_state, Game_State& g_state) noexcept
      : g_state_(&g_state), p_state_(&p_state) {}

    virtual ~Player_State_Impl() noexcept {}

    virtual void step_mouse(Mouse_State const& state) noexcept = 0;

  protected:
    Game_State* g_state_;
    Player_State* p_state_;
  };

  struct Player_State
  {
    Player_State(Game_State& g_state) noexcept;

    void step_mouse(Mouse_State const& state) noexcept
    { impl_->step_mouse(state); }

    template <class T, class... Args>
    void switch_state(Args&&... args) noexcept
    {
      old_impl_ = std::move(impl_);
      impl_=std::make_shared<T>(*this,*g_state_,std::forward<Args>(args)...);
    }
  private:
    std::shared_ptr<Player_State_Impl> impl_;
    std::shared_ptr<Player_State_Impl> old_impl_;

    Game_State* g_state_;
  };

  struct Nothing_State : public Player_State_Impl
  {
    Nothing_State(Player_State& p_state, Game_State& g_state,
                  Vec<float> op = Vec<float>{0.0f,0.0f}) noexcept
      : Player_State_Impl(p_state, g_state), op_(op),
        cam_rot_(*g_state.driver, g_state.cam) {}
    //-----TODO: ^ Should be a reference like camera, etc probably.
    void step_mouse(Mouse_State const& state) noexcept override;

  private:
    bool mouse_down_ = false;
    Vec<float> op_;

    ui::Camera_Rotation_Helper cam_rot_;
  };

  struct Building_State : public Player_State_Impl
  {
    Building_State(Player_State& p_state, Game_State& g_state,
                   Structure& st) noexcept
                   : Player_State_Impl(p_state, g_state), st_(&st) {}

    void step_mouse(Mouse_State const& state) noexcept override;
  private:
    Structure* st_;
  };

  struct Wall_Building_State : public Player_State_Impl
  {
    Wall_Building_State(Player_State& p_state, Game_State& g_state,
                        Wall_Type const& wall_type) noexcept
                        : Player_State_Impl(p_state, g_state),
                          wall_type_(wall_type) {}
    void step_mouse(Mouse_State const& state) noexcept override;
  private:
    int click_index_ = 0;
    Wall_Type wall_type_;
  };
} }

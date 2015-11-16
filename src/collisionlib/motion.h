/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <glm/glm.hpp>
namespace game { namespace collis
{
  struct Displacement_Desc
  {
    glm::vec3 net_force;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    glm::vec3 displacement;
  };

  void reset_force(Displacement_Desc&) noexcept;
  void apply_force(Displacement_Desc&, glm::vec3 force) noexcept;
  void solve_displacement_motion(float dt, float mass,
                                 Displacement_Desc&) noexcept;

  struct Angular_Desc
  {
    float radius;

    glm::vec3 net_torque;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    glm::vec3 displacement;
  };

  void reset_torque(Angular_Desc&) noexcept;
  void apply_torque(Angular_Desc&, glm::vec3 const& r,
                         glm::vec3 const& force) noexcept;
  void solve_angular_motion(float dt, float mass,
                            Angular_Desc&) noexcept;

  struct Motion
  {
    float mass;

    Displacement_Desc displacement;
    Angular_Desc angular;
  };

  void solve_motion(float dt, Motion&) noexcept;
} }

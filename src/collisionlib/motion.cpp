/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "motion.h"
namespace game { namespace collis
{
  void solve_displacement_motion(float dt, float mass,
                                 Displacement_Desc& motion) noexcept
  {
    motion.acceleration = motion.net_force / mass;
    motion.velocity += motion.acceleration * dt;
    motion.displacement += motion.velocity * dt;
  }

  void solve_angular_motion(float dt, float mass,
                            Angular_Desc& motion) noexcept
  {
    // I = mr^2
    float inert_moment = motion.radius * motion.radius * mass;
    motion.acceleration = motion.net_torque / inert_moment;
    motion.velocity += motion.acceleration * dt;
    motion.displacement += motion.velocity * dt;
  }
} }

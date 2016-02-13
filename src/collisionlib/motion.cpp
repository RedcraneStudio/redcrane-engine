/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "motion.h"
namespace redc { namespace collis
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
    motion.acceleration = motion.net_torque / motion.moment_of_inertia;
    motion.velocity += motion.acceleration * dt;
    motion.displacement += motion.velocity * dt;
  }
} }

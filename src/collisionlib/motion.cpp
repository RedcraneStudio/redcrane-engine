/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "motion.h"
namespace game { namespace collis
{
  void reset_force(Displacement_Desc& motion) noexcept
  {
    motion.net_force = glm::vec3(0.0f);
  }
  void apply_force(Displacement_Desc& motion, glm::vec3 force) noexcept
  {
    motion.net_force += force;
  }

  void solve_displacement_motion(float dt, float mass,
                                 Displacement_Desc& motion) noexcept
  {
    motion.acceleration = motion.net_force / mass;
    motion.velocity += motion.acceleration * dt;
    motion.displacement += motion.velocity * dt;
  }

  void reset_torque(Angular_Desc& motion) noexcept
  {
    motion.net_torque = glm::vec3(0.0f);
  }
  void apply_torque(Angular_Desc& motion, glm::vec3 const& r,
                         glm::vec3 const& force) noexcept
  {
    // Cross product yields the correct axis direction and magnitude (angle).
    // Freakin' Cool!
    motion.net_torque += glm::cross(r, force);
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
  void solve_motion(float dt, Motion& motion) noexcept
  {
    solve_displacement_motion(dt, motion.mass, motion.displacement);
    solve_angular_motion(dt, motion.mass, motion.angular);
  }
} }

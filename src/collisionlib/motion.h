/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <glm/glm.hpp>
namespace redc { namespace collis
{
  struct Displacement_Desc
  {
    glm::vec3 net_force{0.0f, 0.0f, 0.0f};
    glm::vec3 acceleration{0.0f, 0.0f, 0.0f};
    glm::vec3 velocity{0.0f, 0.0f, 0.0f};
    glm::vec3 displacement{0.0f, 0.0f, 0.0f};
  };

  void solve_displacement_motion(float dt, float mass,
                                 Displacement_Desc&) noexcept;

  struct Angular_Desc
  {
    float moment_of_inertia;

    glm::vec3 net_torque{0.0f, 0.0f, 0.0f};
    glm::vec3 acceleration{0.0f, 0.0f, 0.0f};
    glm::vec3 velocity{0.0f, 0.0f, 0.0f};
    glm::vec3 displacement{0.0f, 0.0f, 0.0f};
  };

  void solve_angular_motion(float dt, float mass, Angular_Desc&) noexcept;

  struct Motion
  {
    float mass;

    Displacement_Desc displacement;
    Angular_Desc angular;
  };

  inline void solve_motion(float dt, Motion& motion) noexcept
  {
    solve_displacement_motion(dt, motion.mass, motion.displacement);
    solve_angular_motion(dt, motion.mass, motion.angular);
  }

  // Applies a force without a torque.
  inline void apply_force(Motion& motion, glm::vec3 const& force) noexcept
  {
    motion.displacement.net_force += force;
  }

  // Applies a force to a point (model space).
  inline void apply_force(Motion& motion, glm::vec3 const& force,
                   glm::vec3 const& r) noexcept
  {
    apply_force(motion, force);

    // Cross product yields the correct axis direction and magnitude (angle).
    // Freakin' Cool!
    motion.angular.net_torque += glm::cross(r, force);
  }

  // Resets net force to zero
  inline void reset_force(Motion& m) noexcept
  {
    m.displacement.net_force = glm::vec3(0.0f);
    m.angular.net_torque = glm::vec3(0.0f);
  }
} }

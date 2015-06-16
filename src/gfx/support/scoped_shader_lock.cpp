/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "scoped_shader_lock.h"
namespace game { namespace gfx
{
  Shader_Push_Lock::Shader_Push_Lock(Shader& shader, IDriver& driver) noexcept
    : driver_(&driver)
  {
    old_shader_ = driver_->active_shader();
    driver_->use_shader(shader);
  }
  Shader_Push_Lock::Shader_Push_Lock(Shader_Push_Lock&& rhs) noexcept
    : driver_(rhs.driver_), old_shader_(rhs.old_shader_)
  {
    rhs.old_shader_ = nullptr;
  }
  Shader_Push_Lock& Shader_Push_Lock::operator=(Shader_Push_Lock&& l) noexcept
  {
    driver_ = l.driver_;
    old_shader_ = l.old_shader_;

    l.old_shader_ = nullptr;

    return *this;
  }
  Shader_Push_Lock::~Shader_Push_Lock() noexcept
  {
    // For right now, we can't do anything the active shader is a nullptr, as
    // it is impossible to return the driver to that state.
    if(old_shader_) driver_->use_shader(*old_shader_);
  }
} }

/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../ishader.h"
#include "../idriver.h"
namespace redc { namespace gfx
{
  struct Shader_Push_Lock
  {
    Shader_Push_Lock(IShader& shader, IDriver& driver) noexcept;

    Shader_Push_Lock(Shader_Push_Lock&&) noexcept;
    Shader_Push_Lock& operator=(Shader_Push_Lock&&) noexcept;

    ~Shader_Push_Lock() noexcept;
  private:
    IDriver* driver_;
    IShader* old_shader_;
  };

  inline Shader_Push_Lock push_shader(IShader& s, IDriver& d) noexcept
  { return Shader_Push_Lock(s, d); }
} }

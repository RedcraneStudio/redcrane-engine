/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <glm/glm.hpp>
#include "../../common/color.h"
namespace game
{
  namespace gfx
  {
    namespace gl
    {
      // TODO, IDEA: Possibly expose this interface from the Driver. The driver
      // retains the shader's concrete implementation though, so all that good
      // stuff can be used still without re-exporting the interface in the
      // driver so that it can more or less directly translate to a call to
      // one of these functions.
      struct IShader
      {
        virtual void set_projection(glm::mat4 const&) noexcept {}
        virtual void set_view(glm::mat4 const&) noexcept {}
        virtual void set_model(glm::mat4 const&) noexcept {}

        virtual void set_texture(unsigned int) noexcept {}
        virtual void set_diffuse(Color const&) noexcept {}
        virtual void use() noexcept = 0;
      };
    }
  }
}

/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "material.h"
#include <glm/glm.hpp>
#include "prepared_mesh.h"
#include "iscene_observer.h"
namespace strat
{
  namespace gfx
  {
    struct IShader : public IScene_Observer
    {
      virtual ~IShader() noexcept {}

      virtual void set_projection(glm::mat4 const& proj) noexcept = 0;
      virtual void set_view(glm::mat4 const& view) noexcept = 0;
      virtual void set_model(glm::mat4 const& model) noexcept = 0;

      virtual void set_material(Material const&) noexcept = 0;
      virtual void use() noexcept = 0;
      virtual void render(Prepared_Mesh const&) noexcept = 0;
    };
  }
}

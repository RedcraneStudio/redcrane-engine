/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include "prepared_mesh.h"
#include "prepared_texture.h"
namespace survive
{
  namespace gfx
  {
    struct IFactory
    {
      virtual ~IFactory() noexcept {}

      virtual std::unique_ptr<Prepared_Mesh>
        prepare_mesh(Mesh&& mesh) noexcept = 0;
      virtual std::unique_ptr<Prepared_Texture>
        prepare_texture(Texture&& tex) noexcept = 0;
    };
  }
}

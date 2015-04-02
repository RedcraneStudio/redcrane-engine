/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include "prepared_mesh.h"
#include "imaterial.h"
#include "../maybe_owned.hpp"
#include "idriver.h"
namespace survive
{
  namespace gfx
  {
    struct Object
    {
      Maybe_Owned<Prepared_Mesh> mesh;
      Maybe_Owned<IMaterial> material;
    };

    Object create_object(gfx::IDriver& driver, std::string obj,
                         std::string mat) noexcept;

    void render_object(Object const&, glm::mat4 model) noexcept;
  }
}

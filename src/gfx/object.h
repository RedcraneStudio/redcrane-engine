/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include <boost/optional.hpp>
#include "../common/maybe_owned.hpp"
#include "mesh.h"
#include "material.h"
#include "idriver.h"
namespace strat
{
  namespace gfx
  {
    struct Object
    {
      Object() noexcept;

      Maybe_Owned<Mesh> mesh;
      Maybe_Owned<Material> material;

      // Used when a model isn't provided.
      glm::mat4 model_matrix;
    };

    Object load_object(std::string o, std::string m) noexcept;

    void prepare_object(IDriver& d, Object& o) noexcept;
    void remove_object(IDriver& d, Object& o) noexcept;

    void render_object(IDriver& d, Object const&, glm::mat4 model) noexcept;
    void render_object(IDriver& d, Object const&) noexcept;
  }
}

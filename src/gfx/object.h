/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include "prepared_mesh.h"
#include "material.h"
#include "../maybe_owned.hpp"
#include "ifactory.h"
namespace survive
{
  namespace gfx
  {
    struct Object
    {
      Maybe_Owned<Prepared_Mesh> mesh;
      Maybe_Owned<IMaterial> material;

      void render() const noexcept;
    };

    Object create_object(gfx::IFactory& fact, std::string obj,
                         std::string mat) noexcept
    {
      auto ret = Object{};
      ret.mesh = fact.prepare_mesh(Mesh::from_file(obj));
      ret.material = load_material(mat);
      return ret;
    }
  }
}

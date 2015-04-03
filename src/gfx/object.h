/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include <boost/optional.hpp>
#include "prepared_mesh.h"
#include "material.h"
#include "idriver.h"
#include "ishader.h"
namespace survive
{
  namespace gfx
  {
    struct Object
    {
      std::shared_ptr<Prepared_Mesh> mesh;

      std::shared_ptr<IShader> shader;
      boost::optional<Material> material;

      // Used when a model isn't provided.
      boost::optional<glm::mat4> model_matrix;
    };

    Object create_object(gfx::IDriver& d, std::string o,
                         std::string m) noexcept;

    void render_object(Object const&, glm::mat4 model) noexcept;
    void render_object(Object const&) noexcept;
  }
}

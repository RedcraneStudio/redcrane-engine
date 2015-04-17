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
namespace game
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


    // Generates a new separate object but pointing to the other object's
    // mesh and material. No reference counting is being done so this operation
    // can be considered somewhat unsafe.
    Object share_object_keep_ownership(Object const& obj) noexcept;
    // Generates a separate object while owning the other object's mesh and
    // material. The other object is updated to point to what *the returned*
    // object *now owns*. This means the other object being destructed will
    // not delete the mesh and material.
    Object share_object_move_ownership(Object& obj) noexcept;

    void prepare_object(IDriver& d, Object const& o) noexcept;
    void remove_object(IDriver& d, Object const& o) noexcept;

    void render_object(IDriver& d, Object const&, glm::mat4 model) noexcept;
    void render_object(IDriver& d, Object const&) noexcept;
  }
}

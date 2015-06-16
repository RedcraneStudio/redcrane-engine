/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include "mesh.h"
#include "material.h"
#include "../common/maybe_owned.hpp"
#include <boost/optional.hpp>
namespace game
{
  namespace gfx
  {
    struct IDriver;

    struct Object
    {
      Object(IDriver& d) noexcept;

      Object(Object const&) = default;
      Object(Object&&) = default;
      Object& operator=(Object const&) = default;
      Object& operator=(Object&&) = default;

      Maybe_Owned<Mesh> mesh;
      Maybe_Owned<Material> material;

      glm::mat4 model_matrix;

      inline IDriver& driver() const noexcept { return *driver_; }
    private:
      IDriver* driver_;
    };

    // Generates a new separate object but pointing to the other object's
    // mesh and material. No reference counting is being done so this operation
    // can be considered somewhat unsafe.
    Object share_object_keep_ownership(Object const& obj) noexcept;

    // Generates a separate object while owning the other object's mesh and
    // material. The other object is updated to point to what *the returned*
    // object *now owns*. This means the other object being destructed will
    // not delete the mesh and material.
    Object share_object_move_ownership(Object& obj) noexcept;

    void render_object(IDriver& d, Object const&, glm::mat4 model) noexcept;
    void render_object(IDriver& d, Object const& o, glm::mat4 m,
                       std::size_t start, std::size_t count) noexcept;

    void render_object(IDriver& d, Object const&) noexcept;
    void render_object(IDriver& d, Object const&, std::size_t start,
                       std::size_t count) noexcept;
  }
}

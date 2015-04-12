/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */

#include "../gfx/object.h"

#include "catch/catch.hpp"

TEST_CASE("Object moving and sharing works as expected", "[struct Object]")
{
  using namespace strat;
  using namespace strat::gfx;

  Object obj;
  Object other;

  // We just need something unique to check later.
  obj.model_matrix = glm::mat4(10.0);

  SECTION("Original object owns it's data")
  {
    obj.mesh.set_owned(new Mesh());
    obj.material.set_owned(new Material());

    SECTION("Keep ownership")
    {
      other = share_object_keep_ownership(obj);

      REQUIRE(other.mesh.is_pointer());
      REQUIRE(obj.mesh.is_owned());

      REQUIRE(other.material.is_pointer());
      REQUIRE(obj.material.is_owned());

    }
    SECTION("Move ownership")
    {
      other = share_object_move_ownership(obj);

      REQUIRE(other.mesh.is_owned());
      REQUIRE(obj.mesh.is_pointer());

      REQUIRE(other.material.is_owned());
      REQUIRE(obj.material.is_pointer());
    }
  }
  SECTION("Original data doesn't own it's data")
  {
    auto mesh_data = std::make_unique<Mesh>();
    auto mat_data = std::make_unique<Material>();

    obj.mesh.set_pointer(mesh_data.get());
    obj.material.set_pointer(mat_data.get());

    SECTION("Keep pointer-ship")
    {
      other = share_object_keep_ownership(obj);

      REQUIRE(other.mesh.is_pointer());
      REQUIRE(obj.mesh.is_pointer());

      REQUIRE(other.material.is_pointer());
      REQUIRE(obj.material.is_pointer());

    }
    SECTION("Move pointership")
    {
      other = share_object_move_ownership(obj);

      REQUIRE(other.mesh.is_pointer());
      REQUIRE(obj.mesh.is_pointer());

      REQUIRE(other.material.is_pointer());
      REQUIRE(obj.material.is_pointer());
    }
  }

  REQUIRE(other.mesh.get() == obj.mesh.get());
  REQUIRE(other.material.get() == obj.material.get());
  REQUIRE(other.model_matrix == obj.model_matrix);
}

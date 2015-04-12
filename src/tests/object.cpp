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

  obj.mesh.set_owned(new Mesh());
  obj.material.set_owned(new Material());

  // We just need something unique to check later.
  obj.model_matrix = glm::mat4(10.0);

  SECTION("Keep ownership")
  {
    auto other = share_object_keep_ownership(obj);

    REQUIRE(other.mesh.is_pointer());
    REQUIRE(obj.mesh.is_owned());
    REQUIRE(other.mesh.get() == obj.mesh.get());

    REQUIRE(other.material.is_pointer());
    REQUIRE(obj.material.is_owned());
    REQUIRE(other.material.get() == obj.material.get());

    REQUIRE(other.model_matrix == obj.model_matrix);
  }
  SECTION("Move ownership")
  {
    auto other = share_object_move_ownership(obj);

    REQUIRE(other.mesh.is_owned());
    REQUIRE(obj.mesh.is_pointer());
    REQUIRE(other.mesh.get() == obj.mesh.get());

    REQUIRE(other.material.is_owned());
    REQUIRE(obj.material.is_pointer());
    REQUIRE(other.material.get() == obj.material.get());

    REQUIRE(other.model_matrix == obj.model_matrix);
  }
}

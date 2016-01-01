/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */

#include "../gfx/object.h"
#include "../gfx/null/driver.h"

#include "catch/catch.hpp"

TEST_CASE("Object moving and sharing works as expected", "[struct Object]")
{
  using namespace redc;
  using namespace redc::gfx;

  null::Driver driver{{1000,1000}};

  Object obj{driver};
  Object other{driver};

  // We just need something unique to check later.
  obj.model_matrix = glm::mat4(10.0);

  SECTION("Original object owns it's data")
  {
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
    auto mesh_data = driver.make_mesh_repr();
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

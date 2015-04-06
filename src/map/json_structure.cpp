/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "json_structure.h"
#include "../common/json.h"
#include "../mesh.h"
namespace strat
{
  Json_Structure::Json_Structure(gfx::IDriver& driver,
                                 std::string fn, Mesh_Container& mc) noexcept
  {
    auto doc = load_json(fn);

    mesh_id_ = mc.load(doc["mesh"].GetString());
    aabb_ = generate_aabb(*mc.mesh(mesh_id_));

    mat_ = gfx::load_material(driver, doc["material"].GetString());
  }
  AABB Json_Structure::aabb() const noexcept { return aabb_; }

  gfx::Material Json_Structure::mat() const noexcept { return mat_; }
  int Json_Structure::mesh_id() const noexcept { return mesh_id_; }
}

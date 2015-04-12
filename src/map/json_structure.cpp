/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "json_structure.h"
#include "../common/json.h"
#include "../gfx/mesh.h"
namespace strat
{
  Json_Structure::Json_Structure(std::string fn) noexcept
  {
    auto doc = load_json(fn);

    obj_ = gfx::load_object(doc["mesh"].GetString(),
                            doc["material"].GetString());
    aabb_ = generate_aabb(*obj_.mesh);
  }
  AABB Json_Structure::aabb() const noexcept { return aabb_; }

  gfx::Object const& Json_Structure::obj() const noexcept { return obj_; }
}

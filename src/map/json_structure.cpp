/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "json_structure.h"
#include "../common/json.h"
#include "../gfx/mesh.h"
#include <glm/gtc/matrix_transform.hpp>
namespace strat
{
  Json_Structure::Json_Structure(std::string fn) noexcept
  {
    auto doc = load_json(fn);

    obj_ = gfx::load_object(doc["mesh"].GetString(),
                            doc["material"].GetString());
    aabb_ = generate_aabb(*obj_.mesh);

    // Generate a model based on the aabb.
    obj_.model_matrix = glm::translate(glm::mat4(1.0f),
                          glm::vec3(0.0f, 0.0f, 0.0f) - aabb_.min);
  }
  AABB Json_Structure::aabb() const noexcept { return aabb_; }

  gfx::Object Json_Structure::make_obj() const noexcept
  {
    // Use the same mesh and material, we will still own it.
    return share_object_keep_ownership(obj_);
  }
}

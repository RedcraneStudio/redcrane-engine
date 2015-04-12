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
    obj_.model_matrix = gen_model_();
  }
  AABB Json_Structure::aabb() const noexcept { return aabb_; }

  gfx::Object const& Json_Structure::obj() const noexcept { return obj_; }
  void Json_Structure::prepare(gfx::IDriver& driver) noexcept
  {
    prepare_object(driver, obj_);
  }
  void Json_Structure::render(gfx::IDriver& driver) noexcept
  {
    render_object(driver, obj_);
  }
  void Json_Structure::set_model(glm::mat4 const& m) noexcept
  {
    obj_.model_matrix = m * gen_model_();
  }
  glm::mat4 Json_Structure::gen_model_() const noexcept
  {
    return glm::translate(glm::mat4(1.0f),
                          glm::vec3(0.0f, 0.0f, 0.0f) - aabb_.min);
  }
}

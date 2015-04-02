/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "object.h"
namespace survive
{
  namespace gfx
  {
    Object create_object(gfx::IDriver& driver, std::string obj,
                         std::string mat) noexcept
    {
      auto ret = Object{};
      ret.mesh = driver.prepare_mesh(Mesh::from_file(obj));
      ret.material = load_material(mat);
      return ret;
    }
    void render_object(Object const& obj, glm::mat4 model) noexcept
    {
      if(obj.material) obj.material->use(model);
      if(obj.mesh) obj.mesh->render();
    }
  }
}

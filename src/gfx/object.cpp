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
      //ret.material = load_material(driver, mat);
      return ret;
    }
    void render_object(Object const& obj, glm::mat4 model) noexcept
    {
      if(obj.shader)
      {
        obj.shader->set_model(model);
        if(obj.material)
        {
          obj.shader->set_material(*obj.material);
        }
        obj.shader->use();
        if(obj.mesh)
        {
          obj.shader->render(*obj.mesh);
        }
      }
    }
    void render_object(Object const& obj) noexcept
    {
      auto model = glm::mat4(1.0);
      if(obj.model_matrix)
      {
        model = *obj.model_matrix;
      }
      render_object(obj, model);
    }
  }
}

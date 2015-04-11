/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "object.h"
namespace strat
{
  namespace gfx
  {
    Object::Object() noexcept : model_matrix(1.0) {}
    Object create_object(std::string obj, std::string mat) noexcept
    {
      auto ret = Object{};
      ret.mesh = Maybe_Owned<Mesh>(Mesh::from_file(obj));
      ret.material = Maybe_Owned<Material>(load_material(mat));
      return ret;
    }
    void prepare_object(IDriver& d, Object& o) noexcept
    {
      if(o.mesh) d.prepare_mesh(*o.mesh);
      if(o.material) d.prepare_material(*o.material);
    }
    void remove_object(IDriver& d, Object& o) noexcept
    {
      if(o.mesh) d.remove_mesh(*o.mesh);
      if(o.material) d.remove_material(*o.material);
    }

    void render_object(IDriver& d, Object const& o, glm::mat4 m) noexcept
    {
      if(o.material) d.bind_material(*o.material);
      d.set_model(m);
      if(o.mesh) d.render_mesh(*o.mesh);
    }
    void render_object(IDriver& d, Object const& o) noexcept
    {
      render_object(d, o, o.model_matrix);
    }
  }
}

/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "object.h"
namespace game
{
  namespace gfx
  {
    Object::Object() noexcept : model_matrix(1.0) {}
    Object load_object(std::string obj, std::string mat) noexcept
    {
      auto ret = Object{};
      ret.mesh = Maybe_Owned<Mesh>(Mesh::from_file(obj));
      ret.material = Maybe_Owned<Material>(load_material(mat));
      return ret;
    }

    Object share_object_keep_ownership(Object const& obj) noexcept
    {
      auto new_obj = Object{};

      new_obj.mesh.set_pointer(obj.mesh.get());
      new_obj.material.set_pointer(obj.material.get());

      new_obj.model_matrix = obj.model_matrix;

      return new_obj;
    }
    Object share_object_move_ownership(Object& obj) noexcept
    {
      auto new_obj = Object{};

      new_obj.mesh = std::move(obj.mesh);
      new_obj.material = std::move(obj.material);

      obj.mesh.set_pointer(new_obj.mesh.get());
      obj.material.set_pointer(new_obj.material.get());

      new_obj.model_matrix = obj.model_matrix;

      return new_obj;
    }

    void prepare_object(IDriver& d, Object const& o) noexcept
    {
      if(o.mesh) d.prepare_mesh(*o.mesh);
      if(o.material) d.prepare_material(*o.material);
    }
    void remove_object(IDriver& d, Object const& o) noexcept
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

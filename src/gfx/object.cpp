/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "object.h"
#include "../common/mesh_load.h"
namespace game
{
  namespace gfx
  {
    Object::Object() noexcept : model_matrix(1.0)
    {
      mesh = make_maybe_owned<Software_Mesh>();
      material = make_maybe_owned<Material>();
    }
    Object load_object(std::string obj, std::string mat) noexcept
    {
      auto ret = Object{};
      load_obj(obj, *ret.mesh);
      *ret.material = load_material(mat);
      return ret;
    }

    Object share_object_keep_ownership(Object const& obj) noexcept
    {
      auto new_obj = Object{};

      new_obj.mesh.set_pointer(obj.mesh);
      new_obj.material.set_pointer(obj.material);

      new_obj.model_matrix = obj.model_matrix;

      return new_obj;
    }
    Object share_object_move_ownership(Object& obj) noexcept
    {
      auto new_obj = Object{};

      new_obj.mesh = std::move(obj.mesh);
      new_obj.material = std::move(obj.material);

      obj.mesh.set_pointer(new_obj.mesh);
      obj.material.set_pointer(new_obj.material);

      new_obj.model_matrix = obj.model_matrix;

      return new_obj;
    }

    void prepare_object(IDriver& d, Object const& o) noexcept
    {
      // Make sure to prepare this new mesh if the software mesh is prepared.
      o.mesh->set_impl(std::move(d.make_mesh_repr()), true);
    }

    void render_object(IDriver& d, Object const& o, glm::mat4 m) noexcept
    {
      bind_material(d, *o.material);
      d.set_model(m);
      d.render_mesh(o.mesh->get_impl());
    }
    void render_object(IDriver& d, Object const& o) noexcept
    {
      render_object(d, o, o.model_matrix);
    }
  }
}

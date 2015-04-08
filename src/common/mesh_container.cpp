/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "mesh_container.h"
namespace strat
{
  mesh_id_t Mesh_Container::load(std::string fn) noexcept
  {
    auto mesh = Mesh::from_file(fn);
    std::shared_ptr<gfx::Prepared_Mesh> shared_ptr{
                                 driver_->prepare_mesh(std::move(mesh)).get()};
    return meshes_.insert(std::move(shared_ptr));
  }
  Mesh* Mesh_Container::mesh(mesh_id_t id) noexcept
  {
    try
    {
      return &meshes_.find(id)->mesh();
    }
    catch(std::out_of_range& e)
    {
      return nullptr;
    }
  }
  gfx::Prepared_Mesh* Mesh_Container::prepared_mesh(mesh_id_t id) noexcept
  {
    try
    {
      return meshes_.find(id).get();
    }
    catch(std::out_of_range& e)
    {
      return nullptr;
    }
  }
}

/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "software_mesh.h"
namespace game
{
  void Software_Mesh::set_impl(Maybe_Owned<Mesh> m, bool should_prep) noexcept
  {
    // Bail out early, if necessary.
    if(!m) return;

    // If we were prepared with a mesh, we should prepare this child but only
    // if the user specifially told us to do that.
    if(prepared_ && should_prep)
    {
      copy_to(*m);
    }

    impl_ = std::move(m);
  }
  void Software_Mesh::prepare(Mesh_Data const& data) noexcept
  {
    prepared_ = true;
    mesh_data_ = data;
    if(impl_) impl_->prepare(mesh_data_);
  }
  void Software_Mesh::prepare(Mesh_Data&& data) noexcept
  {
    prepared_ = true;

    mesh_data_ = std::move(data);
    // Purposely copy into our child, since we want to maintain ownership to
    // the mesh data.
    if(impl_) impl_->prepare(mesh_data_);
  }
  void Software_Mesh::set_vertices(unsigned int begin, unsigned int length,
                                   Vertex const* src_vertices) noexcept
  {
    mesh_data_.set_vertices(begin, length, src_vertices);
    if(impl_) impl_->set_vertices(begin, length, src_vertices);
  }
}

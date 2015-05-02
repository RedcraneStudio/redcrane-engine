/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "software_mesh.h"
namespace game
{
  void Software_Mesh::set_impl(Maybe_Owned<Mesh> m, bool should_prep) noexcept
  {
    // We are allowing a nullptr to be set to our child, since that would
    // probably indicate the client is looking to destruct the child.

    // If we were prepared with a mesh, we should prepare this child but only
    // if the user specifially told us to do that.
    if(prepared_ && should_prep && m)
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

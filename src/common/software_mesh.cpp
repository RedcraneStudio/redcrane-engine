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

    // If we were allocated with a mesh, we should prepare this child but only
    // if the user specifially told us to do that.
    if(allocated_ && should_prep && m)
    {
      copy_to(*m);
    }

    impl_ = std::move(m);
  }
  void Software_Mesh::allocate_(unsigned int max_verts,
                                unsigned int max_elemnt_indices, Usage_Hint uh,
                                Upload_Hint up, Primitive_Type pt) noexcept
  {
    allocated_ = true;

    mesh_data_.vertices.resize(max_verts);
    mesh_data_.elements.resize(max_elemnt_indices);
    mesh_data_.usage_hint = uh;
    mesh_data_.upload_hint = up;
    mesh_data_.primitive = pt;

    if(impl_) impl_->allocate(max_verts, max_elemnt_indices, uh, up, pt);
  }
  void Software_Mesh::allocate_from_(Mesh_Data const& md) noexcept
  {
    allocated_ = true;

    mesh_data_ = md;
    if(impl_) impl_->allocate_from(mesh_data_);
  }
  void Software_Mesh::allocate_from_(Mesh_Data&& md) noexcept
  {
    allocated_ = true;

    mesh_data_ = std::move(md);
    if(impl_) impl_->allocate_from(mesh_data_);
  }
  void Software_Mesh::set_vertices(unsigned int begin, unsigned int length,
                                   Vertex const* src_vertices) noexcept
  {
    mesh_data_.set_vertices(begin, length, src_vertices);

    if(impl_) impl_->set_vertices(begin, length, src_vertices);
  }
  void Software_Mesh::set_element_indices(unsigned int begin,
                                          unsigned int length,
                                          unsigned int const* indices) noexcept
  {
    mesh_data_.set_element_indices(begin, length, indices);

    if(impl_) impl_->set_element_indices(begin, length, indices);
  }
  void Software_Mesh::set_num_element_indices(unsigned int i) noexcept
  {
    num_indices_ = i;

    if(impl_) impl_->set_num_element_indices(i);
  }
  void Software_Mesh::copy_to(Mesh& other) const noexcept
  {
    other.allocate_from(mesh_data_);
  }
}

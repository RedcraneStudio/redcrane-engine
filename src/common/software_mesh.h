/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "maybe_owned.hpp"
#include "mesh.h"
namespace game
{
  /*!
   * \brief A simple decorater that stores mesh data for access.
   *
   * \note This class and Software_Texture basically are present to host all
   * the good stuff in software so that real implementations don't have to.
   */
  struct Software_Mesh : public Mesh
  {
    Software_Mesh() = default;

    virtual ~Software_Mesh() noexcept {}

    Software_Mesh(Software_Mesh const&) = default;
    Software_Mesh(Software_Mesh&&) noexcept = default;
    Software_Mesh& operator=(Software_Mesh const&) = default;
    Software_Mesh& operator=(Software_Mesh&&) noexcept = default;

    /*!
     * \brief Sets the child of this mesh.
     *
     * \param should_prepare if true, this function will copy it's mesh data to
     * the given child if it has been prepared itself. The child will be
     * prepared after all subsequent calls to this structures prepare method
     * anyway.
     */
    void set_impl(Maybe_Owned<Mesh> m, bool should_prepare = false) noexcept;
    Mesh& get_impl() noexcept { return *impl_; }
    Mesh const& get_impl() const noexcept { return *impl_; }

    void allocate(unsigned int max_verts,
                  unsigned int max_elemnt_indices, Usage_Hint, Upload_Hint,
                  Primitive_Type) noexcept override;

    void allocate_from(Mesh_Data const&) noexcept override;
    void allocate_from(Mesh_Data&&) noexcept override;

    void set_vertices(unsigned int begin,
                      unsigned int length,
                      Vertex const* data) noexcept override;
    void set_element_indices(unsigned int begin,
                             unsigned int length,
                             unsigned int const* indices) noexcept override;
    void set_num_element_indices(unsigned int) noexcept override;

    inline Mesh_Data& mesh_data() noexcept { return mesh_data_; }
    inline Mesh_Data const& mesh_data() const noexcept { return mesh_data_; }

    inline Mesh_Data&& unwrap_mesh_data() noexcept
    { return std::move(mesh_data_); }

    void copy_to(Mesh& other) const noexcept;
  private:
    Maybe_Owned<Mesh> impl_;
    bool allocated_ = false;

    int num_indices_;
    Mesh_Data mesh_data_;
  };
}

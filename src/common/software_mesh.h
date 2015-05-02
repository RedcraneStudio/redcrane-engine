/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "mesh.h"
namespace game
{
  /*!
   * \brief A simple decorater that stores mesh data for access.
   */
  struct Software_Mesh : public Mesh
  {
    virtual ~Software_Mesh() noexcept {}

    /*!
     * \brief Sets the child of this mesh.
     *
     * \params should_prepare if true, this function will copy it's mesh data
     * to the given child if it has been prepared itself. The child will be
     * prepared after all subsequent calls to this structures prepare method
     * anyway.
     */
    void set_impl(Mesh& m, bool should_prepare = false) noexcept;
    Mesh& get_impl() noexcept { return *impl_; }
    Mesh const& get_impl() const noexcept { return *impl_; }

    void prepare(Mesh_Data const& data) noexcept override;
    void prepare(Mesh_Data&& data) noexcept override;
    void set_vertices(unsigned int begin, unsigned int length,
                      Vertex const*) noexcept override;

    inline Mesh_Data& mesh_data() noexcept { return mesh_data_; }
    inline Mesh_Data const& mesh_data() const noexcept { return mesh_data_; }

    inline Mesh_Data&& unwrap_mesh_data() noexcept
    { return std::move(mesh_data_); }

    /*!
     * \brief Constructs a new mesh with a copy of its own mesh data.
     *
     * This function does not depend on an implementation / child.
     */
    inline void copy_to(Mesh& m) const noexcept { m.prepare(mesh_data_); }
  private:
    Mesh* impl_;
    bool prepared_ = false;
    Mesh_Data mesh_data_;
  };
}

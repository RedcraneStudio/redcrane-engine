/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "mesh.h"
namespace game
{
  /*!
   * \brief A simple mesh impl that wraps a mesh_data.
   */
  struct Software_Mesh : public Mesh
  {
    virtual ~Software_Mesh() noexcept {}

    void prepare(Mesh_Data const& data) noexcept override;
    void prepare(Mesh_Data&& data) noexcept override;
    void set_vertices(unsigned int begin, unsigned int length,
                      Vertex const*) noexcept override;

    inline Mesh_Data& mesh_data() noexcept { return mesh_data_; }
    inline Mesh_Data const& mesh_data() const noexcept { return mesh_data_; }

    inline Mesh_Data&& unwrap_mesh_data() noexcept
    { return std::move(mesh_data_); }

    inline void copy_to(Mesh& m) const noexcept { m.prepare(mesh_data_); }
  private:
    Mesh_Data mesh_data_;
  };
}

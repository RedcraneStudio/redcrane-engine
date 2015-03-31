/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "prepared_mesh.h"
#include "../common/log.h"
namespace survive
{
  namespace gfx
  {
    Prepared_Mesh::Prepared_Mesh(Mesh&& mesh) noexcept : mesh_(std::move(mesh))
    {
    }

    Mesh& Prepared_Mesh::mesh() noexcept { return mesh_; }
    Mesh const& Prepared_Mesh::mesh() const noexcept { return mesh_; }

    Mesh&& Prepared_Mesh::unwrap() noexcept
    {
      usable_ = false;
      uninit_();
      return std::move(mesh_);
    }
    void Prepared_Mesh::uninit() noexcept
    {
      if(usable_)
      {
        uninit_();
        usable_ = false;
      }
    }
    void Prepared_Mesh::render() const noexcept
    {
      if(usable_) render_();
#ifndef SURVIVE_RELEASE_BUILD
      else
      {
        log_w("Disregarding attempt to render mesh that isn't usable");
      }
#endif
    }
  }
}

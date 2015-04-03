/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../mesh.h"
namespace survive
{
  namespace gfx
  {
    struct Prepared_Mesh
    {
      Prepared_Mesh(Mesh&&) noexcept;
      virtual ~Prepared_Mesh() noexcept {}

      Mesh& mesh() noexcept;
      Mesh const& mesh() const noexcept;

      Mesh&& unwrap() noexcept;
      void render() const noexcept;
    protected:
      void uninit() noexcept;
    private:
      Mesh mesh_;
      bool usable_ = true;
      virtual void render_() const noexcept = 0;

      // Must be called in the deriving class's destructor.
      virtual void uninit_() noexcept = 0;
    };
  }
}

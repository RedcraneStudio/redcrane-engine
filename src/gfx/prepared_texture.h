/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../texture.h"
namespace survive
{
  namespace gfx
  {
    struct Prepared_Texture
    {
      Prepared_Texture(Texture&&) noexcept;
      virtual ~Prepared_Texture() noexcept {}

      Texture& texture() noexcept;
      Texture const& texture() const noexcept;

      Texture&& unwrap() noexcept;

      void bind(unsigned int loc) const noexcept;
    protected:
      void uninit() noexcept;
    private:
      Texture tex_;
      bool usable_ = true;
      virtual void bind_(unsigned int loc) const noexcept = 0;

      // Must be called in the deriving class's destructor.
      virtual void uninit_() noexcept = 0;
    };
  }
}

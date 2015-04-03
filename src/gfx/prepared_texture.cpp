/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "prepared_texture.h"
#include "../common/log.h"
namespace survive
{
  namespace gfx
  {
    Prepared_Texture::Prepared_Texture(Texture&& tmp_tex) noexcept
      : tex_(std::move(tmp_tex))
    {
    }

    Texture& Prepared_Texture::texture() noexcept { return tex_; }
    Texture const& Prepared_Texture::texture() const noexcept { return tex_; }

    Texture&& Prepared_Texture::unwrap() noexcept
    {
      usable_ = false;
      uninit_();
      return std::move(tex_);
    }
    void Prepared_Texture::uninit() noexcept
    {
      if(usable_)
      {
        uninit_();
        usable_ = false;
      }
    }
    void Prepared_Texture::bind(unsigned int loc) const noexcept
    {
      if(usable_) bind_(loc);
#ifndef SURVIVE_RELEASE_BUILD
      else
      {
        log_w("Disregarding attempt to use a texture that isn't usable");
      }
#endif
    }
  }
}

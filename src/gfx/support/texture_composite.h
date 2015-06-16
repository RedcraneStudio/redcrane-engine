/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../texture.h"
#include "../../common/maybe_owned.hpp"
namespace game
{
  struct Texture_Composite : public Texture
  {
    template <class Iter>
    Texture_Composite(Iter begin, Iter end) noexcept;

    inline Texture_Composite(std::vector<Maybe_Owned<Texture> >&& ts) noexcept
      : ts_(std::move(ts)) {}

    Texture_Composite(Texture_Composite&&) noexcept;
    Texture_Composite& operator=(Texture_Composite&&) noexcept;

    // We could add copy semantics if we add a clone function to the texture
    // interface, but that doesn't seem like a good idea for other reasons.
    // We'll stick with simply move semantics for now.

    virtual ~Texture_Composite() noexcept {}
  private:
    void allocate_(Vec<int> const& extents) noexcept override;
    void blit_data_(Volume<int> const& vol,
                    Color const* data) noexcept override;

    std::vector<Maybe_Owned<Texture> > ts_;
  };

  template <class... Args>
  Texture_Composite make_texture_composite(Args*... args) noexcept
  {
    std::vector<Maybe_Owned<Texture> > ts { Maybe_Owned<Args>(args, false)... };
    Texture_Composite(std::move(ts));
  }
}

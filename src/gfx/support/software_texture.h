/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../texture.h"
#include "../../common/maybe_owned.hpp"
namespace game
{
  struct Software_Texture : public Texture
  {
    Software_Texture() noexcept;
    virtual ~Software_Texture() noexcept;

    void set_impl(Maybe_Owned<Texture> t, bool should_blit = false) noexcept;
    Texture* get_impl() noexcept { return impl_.get(); }
    Texture const* get_impl() const noexcept { return impl_.get(); }

    Color get_pt(Vec<int> pt) const noexcept;
    Color* get_row(int row) const noexcept;
    Color* get_data() const noexcept;

    void allocate_to(Texture& t) const noexcept;
    void blit_to(Texture& t) const noexcept;
  private:
    void allocate_(Vec<int> const& extents, Texture_Format) noexcept override;
    void blit_data_(Volume<int> const& vol,
                    Color const* data) noexcept override;
    void blit_data_(Volume<int> const& vol,
                    float const* data) noexcept override;

    Color* data_;

    Maybe_Owned<Texture> impl_;
  };
}

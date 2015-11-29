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

    inline Image_Format format() const noexcept { return format_; }
    float const* get_pt(Vec<int> pt) const noexcept;

    void allocate_to(Texture& t) const noexcept;
    void blit_to(Texture& t) const noexcept;
  private:
    void allocate_(Vec<int> const&, Image_Format) noexcept override;

    void blit_data_(Volume<int> const&, Color const* data) noexcept override;

    void blit_data_(Volume<int> const&, float const*) noexcept override;

    void blit_data_(Volume<int> const&, Data_Type, void const*) noexcept override;

    std::size_t elements_per_pixel() const noexcept;

    // Internal format always floats
    float* data_;
    // Basically used to determine how many floats per pixel / element.
    Image_Format format_;

    Maybe_Owned<Texture> impl_;
  };
}

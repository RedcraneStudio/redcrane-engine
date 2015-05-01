/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "texture.h"
namespace game
{
  struct Software_Texture : public Texture
  {
    virtual ~Software_Texture() noexcept;

    Color get_pt(Vec<int> pt) const noexcept;
    Color* get_row(int row) const noexcept;
    Color* get_data() const noexcept;

    void allocate_to(Texture& t) const noexcept;
    void blit_to(Texture& t) const noexcept;
  private:
    void allocate_(Vec<int> const& extents) noexcept override;
    void blit_data_(Volume<int> const& vol,
                    Color const* data) noexcept override;

    Color* data_;
  };
}

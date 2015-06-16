/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "software_texture.h"
namespace game
{
  Software_Texture::Software_Texture() noexcept
  {
    data_ = nullptr;
  }
  Software_Texture::~Software_Texture() noexcept
  {
    delete[] data_;
  }
  void Software_Texture::set_impl(Maybe_Owned<Texture> t, bool do_b) noexcept
  {
    if(do_b && t && data_)
    {
      allocate_to(*t);
      blit_to(*t);
    }
    impl_ = std::move(t);
  }
  Color Software_Texture::get_pt(Vec<int> pt) const noexcept
  {
    return data_[pt.y * allocated_extents().x + pt.x];
  }
  Color* Software_Texture::get_row(int row) const noexcept
  {
    return data_ + row * allocated_extents().x;
  }
  Color* Software_Texture::get_data() const noexcept
  {
    return data_;
  }

  void Software_Texture::allocate_to(Texture& t) const noexcept
  {
    // Allocate the other texture with our bounds to prepare it for blitting.
    t.allocate(allocated_extents());
  }
  void Software_Texture::blit_to(Texture& t) const noexcept
  {
    // Blit all of our data to the given texture.
    t.blit_data(vol_from_extents(allocated_extents()), data_);
  }

  void Software_Texture::allocate_(Vec<int> const& extents) noexcept
  {
    data_ = new Color[extents.x * extents.y];
    if(impl_)
    {
      impl_->allocate(extents);
    }
  }
  void Software_Texture::blit_data_(Volume<int> const& vol,
                                    Color const* cd) noexcept
  {
    auto extents = vol_extents(vol);
    for(int i = 0; i < extents.y; ++i)
    {
      for(int j = 0; j < extents.x; ++j)
      {
        data_[(vol.pos.y + i) * allocated_extents().x + vol.pos.x + j] =
          cd[i * extents.x + j];
      }
    }
    if(impl_)
    {
      impl_->blit_data(vol, cd);
    }
  }
}

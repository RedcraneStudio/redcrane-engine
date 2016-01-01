/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "software_texture.h"
namespace redc
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
  float const* Software_Texture::get_pt(Vec<int> pt) const noexcept
  {
    return data_ + (pt.y * allocated_extents().x * elements_per_pixel() + pt.x);
  }

  void Software_Texture::allocate_to(Texture& t) const noexcept
  {
    // Allocate the other texture with our bounds to prepare it for blitting.
    t.allocate(allocated_extents(), format_);
  }

  void Software_Texture::blit_to(Texture& t) const noexcept
  {
    // Blit all of our data to the given texture.
    t.blit_data(vol_from_extents(allocated_extents()), data_);
  }

  void Software_Texture::allocate_(Vec<int> const& extents,
                                   Image_Format format) noexcept
  {
    // Ignore the format for ourselves.
    data_ = new float[extents.x * extents.y * elements_per_pixel()];
    format_ = format;

    if(impl_)
    {
      impl_->allocate(extents, format);
    }
  }
  void Software_Texture::blit_data_(Volume<int> const& vol,
                                    Color const* cd) noexcept
  {
    blit_data_(vol, Data_Type::Integer, cd);
  }
  void Software_Texture::blit_data_(Volume<int> const& vol,
                                    float const* cd) noexcept
  {
    blit_data_(vol, Data_Type::Float, cd);
  }
  void Software_Texture::blit_data_(Volume<int> const& vol, Data_Type type,
                                    void const* in_data) noexcept
  {
    if(type == Data_Type::Float)
    {
      auto cd = (float const*) in_data;

      auto extents = vol_extents(vol);
      for(int i = 0; i < extents.y; ++i)
      {
        for(int j = 0; j < extents.x; ++j)
        {
          for(std::size_t elem = 0; elem < elements_per_pixel(); ++elem)
          {
            data_[(vol.pos.y + i) * allocated_extents().x + vol.pos.x + j+elem] =
              cd[i * extents.x + j + elem];
          }
        }
      }
      if(impl_)
      {
        impl_->blit_data(vol, cd);
      }
    }
    if(type == Data_Type::Integer)
    {
      auto cd = (uint8_t const*) in_data;

      auto extents = vol_extents(vol);
      for(int i = 0; i < extents.y; ++i)
      {
        for(int j = 0; j < extents.x; ++j)
        {
          for(std::size_t elem = 0; elem < elements_per_pixel(); ++elem)
          {
            data_[(vol.pos.y + i) * allocated_extents().x + vol.pos.x + j+elem] =
              cd[i * extents.x + j + elem] / (float) 0xff;
          }
        }
      }
    }
    if(impl_)
    {
      impl_->blit_data(vol, type, in_data);
    }
  }
  std::size_t Software_Texture::elements_per_pixel() const noexcept
  {
    return format_ == Image_Format::Rgba? 4 : 1;
  }
}

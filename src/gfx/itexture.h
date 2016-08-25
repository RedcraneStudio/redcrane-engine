/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once

#include <vector>

#include "../common/vec.h"
#include "../common/volume.h"
#include "../common/color.h"

#include "common.h"
#include "ihandle.h"
namespace redc { namespace gfx
{
  struct ITexture : public IHandle
  {
    virtual ~ITexture() {}

    void allocate(Vec<std::size_t> const& extents,
                  Texture_Format form = Texture_Format::Rgba,
                  Texture_Target type = Texture_Target::Tex_2D);

    virtual Texture_Target target() const { return target_; }

    virtual void blit_tex2d_data(Volume<std::size_t> const& vol,
                                 Texture_Format format, Data_Type type,
                                 void const* data) = 0;
    virtual void blit_cube_data(Cube_Map_Texture const& side,
                                Volume<std::size_t> const& vol,
                                Texture_Format format, Data_Type type,
                                void const* data) = 0;

    inline Vec<int> allocated_extents() const { return extents_; }

    virtual void set_mag_filter(Texture_Filter filter) = 0;
    virtual void set_min_filter(Texture_Filter filter) = 0;
    virtual void set_wrap_s(Texture_Wrap wrap) = 0;
    virtual void set_wrap_t(Texture_Wrap wrap) = 0;
    virtual void set_wrap_r(Texture_Wrap wrap) = 0;
    virtual void set_mipmap_level(unsigned int level) = 0;

  private:
    virtual void allocate_(Vec<std::size_t> const&, Texture_Format,
                           Texture_Target) = 0;

    Texture_Target target_;
    Vec<int> extents_;
  };
} }

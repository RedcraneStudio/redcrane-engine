/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "sprite.h"

#include "../../common/center.hpp"
namespace game { namespace ui
{
  Volume<int> Sprite::get_src_rect() const noexcept
  {
    auto vol = Volume<int>{};
    if(src_vol_) vol = src_vol_.value();
    return vol;
  }

  Vec<int> Sprite::get_minimum_extents_() const noexcept
  {
    auto extents = src_->allocated_extents();
    extents.x *= scale_;
    extents.y *= scale_;
    return extents;
  }
  Volume<int> Sprite::layout_()
  {
    return center_volume(parent_volume(), get_minimum_extents());
  }
  void Sprite::render_(Renderer& r) const noexcept
  {
    if(!dst_tex_)
    {
      // Make a new texture.
      dst_tex_ = r.make_texture();

      // Blit our shit to it
      if(src_)
      {
        src_->allocate_to(*dst_tex_.get());
        src_->blit_to(*dst_tex_.get());
      }
    }

    // Center the image in using our image scale.
    auto dst = center_volume(this_volume(), get_minimum_extents());
    // Draw that texture.
    r.draw_texture(dst, *dst_tex_);
  }
} }

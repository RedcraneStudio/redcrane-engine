/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "sprite.h"

#include "../../common/center.hpp"
namespace game { namespace ui
{
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
    if(!src_) return;

    // This prohibits changing renderers mid-lifetime. The correct way is to
    // cache it depending on the renderer pointer. So like, when a new pointer
    // is passed this function just chucks it's driver-specific texture and
    // rebuilds it.
    if(!src_->get_impl())
    {
      // Make a new texture and blit our shit to it.
      src_->set_impl(r.make_texture(), true);
    }

    // Center the image in using our image scale.
    auto dst = center_volume(this_volume(), get_minimum_extents());
    // Draw that texture.
    r.set_draw_color(color_);
    if(src_vol_) r.draw_texture(dst, *src_->get_impl(), src_vol_.get());
    else r.draw_texture(dst, *src_->get_impl());
  }
} }

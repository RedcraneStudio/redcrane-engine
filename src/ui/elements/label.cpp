/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "label.h"
namespace game { namespace ui
{
  Vec<int> Label::get_minimum_extents_() const noexcept
  {
    return font_->query_size(full_str_(), size_);
  }
  Volume<int> Label::layout_()
  {
    return center_volume(parent_volume(), get_minimum_extents());
  }
  void Label::render_(Renderer& r) const noexcept
  {
    gen_cache_(r);

    r.set_draw_color(col_);

    // Center the texture in our volume.
    auto dst = center_volume(this_volume(), tex_->allocated_extents());
    r.draw_texture(dst, *tex_.get());
  }
  std::string Label::full_str_() const noexcept
  {
    std::string str = str_;
    if(1 <= str.size()) if(str[0] == '@')
    {
      // We have a string name from the lang file.
      // So do the translation.

      str = args_->translate(str.substr(1));
    }
    return str;
  }
  void Label::invalidate_cache_() const noexcept
  {
    tex_.reset();
  }
  void Label::gen_cache_(Renderer& r) const noexcept
  {
    if(!tex_)
    {
      // Get a new texture builder.
      tex_ = r.make_texture();
      // Render the text to it.
      font_->text(full_str_(), size_, *tex_.get());
    }
  }
} }

/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include <boost/optional.hpp>
#include "../element.h"
#include "../../gfx/support/software_texture.h"
namespace game { namespace ui
{
  struct Sprite : public Element
  {
    inline double scale() const noexcept { return scale_; }
    inline void scale(double scale) noexcept { scale_ = scale; }

    inline void src(std::shared_ptr<Software_Texture> tex) noexcept
    { src_ = tex; }
    inline std::shared_ptr<Software_Texture> src() const noexcept
    { return src_; }

    inline void set_src_rect(Volume<int> vol) noexcept { src_vol_ = vol; }
    Volume<int> get_src_rect() const noexcept
    { return src_vol_ ? src_vol_.value() : Volume<int>{}; }

    inline void color(Color const& c) noexcept
    { color_ = c; }
    inline Color color() const noexcept
    { return color_; }

    inline void remove_src_rect() noexcept { src_vol_ = boost::none; }

  private:
    Vec<int> get_minimum_extents_() const noexcept override;
    Volume<int> layout_() override;
    void render_(Renderer&) const noexcept override;

    std::shared_ptr<Software_Texture> src_;
    boost::optional<Volume<int> > src_vol_;
    Color color_ = colors::white;

    double scale_ = 1.0;
  };
} }

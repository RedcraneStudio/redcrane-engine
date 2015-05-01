/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include <boost/optional.hpp>
#include "../element.h"
#include "../../common/software_texture.h"
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
    Volume<int> get_src_rect() const noexcept;
    inline void remove_src_rect() noexcept { src_vol_ = boost::none; }

  private:
    Vec<int> get_minimum_extents_() const noexcept override;
    Volume<int> layout_() override;
    void render_(Renderer&) const noexcept override;
    void activate_regions_(Controller&) const noexcept override {}

    std::shared_ptr<Software_Texture> src_;
    boost::optional<Volume<int> > src_vol_;

    mutable std::shared_ptr<Texture> dst_tex_;

    double scale_ = 1.0;
  };
} }

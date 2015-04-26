/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <memory>
#include "../View.h"
#include "../../common/surface.h"
#include "../../assets/assets.h"
namespace game { namespace ui
{
  struct Sprite : public View
  {
    using Image_Asset_Ptr = std::shared_ptr<assets::Image_Asset>;

    Sprite(Graphics_Desc& g, Image_Asset_Ptr asset = nullptr) noexcept;

    inline double scale() const noexcept { return scale_; }
    inline void scale(double scale) noexcept { scale_ = scale; }

    void src(Image_Asset_Ptr ptr) noexcept;
    inline Image_Asset_Ptr src() const noexcept { return src_; }

    void set_src_rect(Volume<int> vol) noexcept;
    Volume<int> get_src_rect() const noexcept;
    void remove_src_rect() noexcept;

  private:
    Vec<int> get_minimum_extents_() const noexcept override;
    Volume<int> layout_() override;
    void render_() const noexcept override;

    boost::optional<Volume<int> > src_vol_;

    Image_Asset_Ptr src_;

    double scale_ = 1.0;
  };
} }

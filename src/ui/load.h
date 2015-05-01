/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>

#include "cast.h"
#include "element.h"

#include "ifont_renderer.h"
#include "renderer.h"
namespace game { namespace ui
{
  struct Bad_Orientation{};

  struct Invalid_Alignment
  {
    std::string align_str;
  };

  struct Load_Params
  {
    IFont_Renderer& font_render;
    Renderer& renderer;
  };

  Shared_Element load(std::string fn, Load_Params params) noexcept;

  template <typename T>
  inline std::shared_ptr<T> load_as(std::string fn, Load_Params p) noexcept
  {
    return as<T>(load(fn, p));
  }
} }

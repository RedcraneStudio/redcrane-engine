/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "renderer.h"
namespace redc { namespace ui
{
  struct IFont_Renderer
  {
    virtual void text(std::string const&, int size, Texture& tb) noexcept = 0;
    virtual Vec<int> query_size(std::string const&, int size) noexcept = 0;
  };
} }

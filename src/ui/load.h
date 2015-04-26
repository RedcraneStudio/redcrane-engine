/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <string>
#include "../game/Game.h"

#include "cast.h"
#include "View.h"
namespace game { namespace ui
{
  struct Bad_Orientation{};

  struct Invalid_Alignment
  {
    std::string align_str;
  };

  Shared_View load(Game&, std::string name) noexcept;

  template <typename T>
  inline std::shared_ptr<T> load_as(Game& g, std::string name) noexcept
  {
    return as<T>(load(g, name));
  }
} }

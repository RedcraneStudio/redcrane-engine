/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <cstdint>
namespace game
{
  struct Color
  {
    uint8_t r;
    uint8_t g;
    uint8_t b;
  };

  inline bool operator==(Color const& c1, Color const& c2) noexcept
  {
    return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
  }
  inline bool operator!=(Color const& c1, Color const& c2) noexcept
  {
    return !(c1 == c2);
  }

  namespace colors
  {
    constexpr Color white {0xff, 0xff, 0xff};
    constexpr Color black {0x00, 0x00, 0x00};
    constexpr Color red   {0xff, 0x00, 0x00};
    constexpr Color green {0x00, 0xff, 0x00};
    constexpr Color blue  {0x00, 0x00, 0xff};
  }
}

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
    // CTs WIN!
    using c_t = uint8_t;

    constexpr Color(c_t r = 0xff, // <- Default to some weird salmon color so
                    c_t g = 0x8f, // we know when the client forgot to
                    c_t b = 0xab, // initialize it's color!
                    c_t a = 0xff) noexcept : r(r), g(g), b(b), a(a) {}

    c_t r;
    c_t g;
    c_t b;
    c_t a;
  };

  inline bool operator==(Color const& c1, Color const& c2) noexcept
  {
    return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a;
  }
  inline bool operator!=(Color const& c1, Color const& c2) noexcept
  {
    return !(c1 == c2);
  }

  namespace colors
  {
    constexpr Color white {0xff, 0xff, 0xff, 0xff};
    constexpr Color black {0x00, 0x00, 0x00, 0xff};
    constexpr Color red   {0xff, 0x00, 0x00, 0xff};
    constexpr Color green {0x00, 0xff, 0x00, 0xff};
    constexpr Color blue  {0x00, 0x00, 0xff, 0xff};
  }
}

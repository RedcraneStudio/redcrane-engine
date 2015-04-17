/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "translate.h"
namespace game
{
  Lang language_;

  void set_lang(Lang const& lang) noexcept
  {
    language_ = lang;
  }
  Lang get_lang() noexcept
  {
    return language_;
  }
}

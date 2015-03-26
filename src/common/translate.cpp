/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#include "translate.h"
namespace survive
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

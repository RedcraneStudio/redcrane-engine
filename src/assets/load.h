/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <vector>
#include <string>
#include <memory>

#include "../gfx/idriver.h"
#include "asset.h"

namespace survive { namespace assets
{
  /*!
   * \brief Filters the list of pathnames and loads them into an asset
   * implementation.
   *
   * \returns A vector containing smart pointers to each asset.
   *
   * \note Currently supported asset types: json, png, and ttf.

   * \sa game::get_asset for searching the result of this function.
   */
  assets::Vector load(std::vector<std::string> const& assets,
                      gfx::IDriver& fact) noexcept;
} }

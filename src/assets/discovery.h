/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <vector>
#include <string>
namespace game { namespace assets
{
  /*!
   * \brief Scans a directory for any files.
   *
   * \returns A vector of filepaths relative to the subdirectory passed into
   * the function.
   */
  auto discover(std::string subdir) noexcept -> std::vector<std::string>;
} }

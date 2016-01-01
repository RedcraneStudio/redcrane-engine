/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include "../shader.h"
#include <unordered_map>
namespace redc { namespace gfx
{
  struct Location_Cache
  {
    Location_Cache(Shader& shader) : shader_(&shader) {}

    int operator()(std::string const&) const noexcept;
  private:
    Shader* shader_;
    mutable std::unordered_map<std::string, int> cache_;
  };
} }

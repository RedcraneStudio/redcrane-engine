/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "tree.h"
namespace game
{
  unsigned int tree_amount_nodes(unsigned int N, std::size_t depth) noexcept
  {
    if(depth == 0) return 0;

    std::size_t sum = 0;
    for(std::size_t i = 0; i < depth; ++i)
    {
      sum += pow(N, i);
    }
    return sum;
  }
  std::size_t tree_level_offset(unsigned int N, std::size_t level) noexcept
  {
    if(level == 0) return 0;

    std::size_t sum = 1;
    for(std::size_t i = 0; i < level-1; ++i)
    {
      sum += pow(N, i+1);
    }
    return sum;
  }
}

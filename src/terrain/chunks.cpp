/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "chunks.h"
namespace game { namespace terrain
{
  // Done once, hopefully not that awful in terms of performance
  Heightmap make_heightmap(gen::Grid_Map const& map) noexcept
  {
    Heightmap ret;
    ret.allocate(map.extents);

    for(int i = 0; i < map.extents.x * map.extents.y; ++i)
    {
      float value = 0.0f;

      switch(map.values[i].type)
      {
      case gen::Cell_Type::Water:
        value = 0.0f;
        break;
      case gen::Cell_Type::Land:
        value = 1.0f;
      }

      ret.values[i] = value;
    }

    return ret;
  }
} }

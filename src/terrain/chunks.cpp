/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "chunks.h"
namespace game { namespace terrain
{
  //! Wat
  int distance_from_water(Vec<int> pos, gen::Grid_Map const& map,
                          int max_dist) noexcept
  {
    int found_at = 0;

    // We found a distance, get outta here.
    for(int i = 1; i <= max_dist && !found_at; ++i)
    {
      if(pos.x - i < 0 || pos.x + i > map.extents.x ||
         pos.y - i < 0 || pos.y + i > map.extents.y) continue;

      // pos needs to go from {pos.x - i, pos.y - i} to {pos.x + i, pos.y + i}
      // and back around the other way for each distance level i.

      // # = path
      // # # # # #
      // # - - - #
      // # - - - #
      // # - - - #
      // # # # # #
      // i = 2 in the case above

      // Simplification of (i * 2) + 1 + (i * 2) + (i * 2) + (i * 2) - 1
      // (i * 2) + 1: The amount of pixels on top
      // (i * 2) + (i * 2): On both sides
      // (i * 2) - 1: On the bottom
      for(int j = 0; j < 4 * (i * 2); ++j)
      {
        // At the top
        if(j < (i * 2) + 1)
        {
          // Start here .
          // .----------^
          // # # # # #
          // # - - - #
          // # - - - #
          // # - - - #
          // # # # # #
          auto off = j % ((i * 2) + 1);
          if(map.at({pos.x - i + off, pos.y - i}).type == gen::Cell_Type::Water)
          {
            found_at = i;
          }
        }
        // At the right
        else if(j < 2 * (i * 2) + 1)
        {
          // Start here -.
          //             |
          // # # # # #   |
          // # - - - # <-^
          // # - - - #
          // # - - - #
          // # # # # #
          auto off = j % (2 * (i * 2) + 1);
          if(map.at({pos.x + i, pos.y - i + off + 1}).type == gen::Cell_Type::Water)
          {
            found_at = i;
          }
        }
        // Search the left
        else if(j < 3 * (i * 2) + 1)
        {
          // Start here .
          // .----------^
          // |   # # # # #
          // ^-> # - - - #
          //     # - - - #
          //     # - - - #
          //     # # # # #
          auto off = j % (3 * (i * 2) + 1);
          if(map.at({pos.x - i, pos.y - i + off + 1}).type == gen::Cell_Type::Water)
          {
            found_at = i;
          }
        }
        // Search on the bottom
        else
        {
          // Start here .
          // .----------^
          // |   # # # # #
          // |   # - - - #
          // |   # - - - #
          // |   # - - - #
          // |   # # # # #
          // ^----^    ^
          // _____|    |_____
          // | And go to... |
          // ----------------
          auto off = j % (4 * (i * 2));
          if(map.at({pos.x - i + off + 1, pos.y + i}).type == gen::Cell_Type::Water)
          {
            found_at = i;
          }
        }
      }
    }

    return found_at;
  }

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
        {
          // Get the position in cartesian coordinates.
          auto pos = Vec<int>{i % map.extents.x, i / map.extents.x};
          // Get the distance of the current point from water.
          auto distance = distance_from_water(pos, map, 5);
          // If it returns zero that means we are not close at all.
          if(!distance) value = 1.0f;
          // Otherwise the shorter the distance the less height we give the
          // location on the heightmap.
          else value = 1.0f - distance / 5.0f;
          break;
        }
      }

      ret.values[i] = value;
    }

    return ret;
  }
} }

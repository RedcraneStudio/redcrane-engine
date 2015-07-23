/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */

#include <iostream>
#include "stratlib/event.h"
#include <thread>

constexpr char const* const RESET_C = "\x1b[0m";
constexpr char const* const RED_C = "\x1b[95m";
constexpr char const* const CLEAR_C = "\x1b[2J";

int main(int argc, char** argv) noexcept
{
  using namespace game;

  auto const map_size = Vec<int>{50,50};

  strat::Cost_Map cm; cm.allocate(map_size);

  strat::Event_Map event_map;
  event_map.active_events.push_back(Vec<int>{0,0});
  event_map.visited_map.allocate(map_size);

  strat::Value_Map<char> cmap;
  cmap.allocate(map_size);

  for(int i = 0; i < 100; ++i)
  {
    spread(cm, event_map);

    // Set some cells to pound signs.
    for(int i = 0; i < cmap.extents.y; ++i)
    {
      for(int j = 0; j < cmap.extents.x; ++j)
      {
        cmap.at({j,i}) = event_map.visited_map.at({j,i}) ? '#' : ' ';
      }
    }

    // Print the grid
    std::cout << RED_C << i << RESET_C << std::endl;
    for(int i = 0; i < cmap.extents.y; ++i)
    {
      for(int j = 0; j < cmap.extents.x; ++j)
      {
        std::cout << cmap.at({i,j});
      }
      std::cout << std::endl;
    }
    std::cout << CLEAR_C;
  }

  return 0;
}

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

  strat::Cost_Map cm;

  strat::Event_Map event_map;
  event_map.active_events.push_back(Vec<int>{0,0});

  for(int i = 0; i < 50; ++i)
  {
    spread(cm, event_map);

    // Initialize the grid.
    strat::Value_Map<char> cmap;
    cmap.allocate(Vec<int>{20,20});
    for(int i = 0; i < cmap.extents.x * cmap.extents.y; ++i)
    {
      cmap.values[i] = ' ';
    }

    // Set some cells to pound signs.
    for(auto pos : event_map.visited_events)
    {
      pos.x += 10;
      pos.y += 10;
      if(pos.x < 0 || 20 <= pos.x || pos.y < 0 || 20 <= pos.y) continue;
      std::cout << pos.x << ',' << pos.y << std::endl;
      cmap.at(pos) = '#';
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
